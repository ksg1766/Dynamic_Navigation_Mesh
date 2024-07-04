#include "stdafx.h"
#include "..\Public\Strife_Ammo_Static.h"
#include "GameInstance.h"
#include "Layer.h"
#include "MonsterController.h"
#include "Strife_Ammo_Static.h"

_float2	CStrife_Ammo_Static::m_UVoffset = _float2(0.f, 0.f);
_int	CStrife_Ammo_Static::m_iLightningSwitch = 0.f;
_float	CStrife_Ammo_Static::m_fFR_Default = 0.2f;
_float	CStrife_Ammo_Static::m_fFR_Default_Timer = 0.f;

CStrife_Ammo_Static::CStrife_Ammo_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_Ammo_Static::CStrife_Ammo_Static(const CStrife_Ammo_Static& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_Ammo_Static::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Static::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	return S_OK;
}

void CStrife_Ammo_Static::Tick(const _float& fTimeDelta)
{
	if (m_IsChain)
	{
		m_pGameInstance->DeleteObject(this);
		return;
	}
	Super::Tick(fTimeDelta);
	FindTargets();

	// Temp
	if (m_vecTargets.empty())
	{
		m_pGameInstance->DeleteObject(this);
		return;
	}
	CTransform* pTransform = GetTransform();
	CGameObject*& pFirstTarget = m_vecTargets.front();

	const Vec3& vPos = pTransform->GetPosition();
	const Vec3& vTargetPos = pFirstTarget->GetTransform()->GetPosition();

	_float fLength = (vTargetPos - vPos).Length();
	pTransform->SetScale(Vec3(0.5f, fLength, 1.f));
	Vec3 vOffset = (vTargetPos - vPos) / 2.f;
	vOffset.y = 0.f;
	pTransform->Translate(vOffset);

	Attack(this, pFirstTarget, fTimeDelta);
	m_pGameInstance->DeleteObject(this);

	for (_int i = 0; i < m_vecTargets.size() - 1; ++i)
	{
		CStrife_Ammo_Static* pAmmo = static_cast<CStrife_Ammo_Static*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Strife_Ammo_Static"), LAYERTAG::EQUIPMENT, &m_tProps));
		//CStrife_Ammo_Static* pAmmo = static_cast<CStrife_Ammo_Static*>(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYERTAG::EQUIPMENT, TEXT("Prototype_GameObject_Strife_Ammo_Static"), &m_tProps));
		pAmmo->m_IsChain = true;

		CTransform* pNewTransform = pAmmo->GetTransform();
		//pNewTransform->SetPosition(m_vecTargets[i]->GetTransform()->GetPosition());
		CGameObject*& pNextTarget = m_vecTargets[i + 1];

		const Vec3& vNewPos = pNewTransform->GetPosition();
		const Vec3& vNextTargetPos = pNextTarget->GetTransform()->GetPosition();
		Vec3 vNewForward(vNextTargetPos - m_vecTargets[i + 1]->GetTransform()->GetPosition());

		_float fLength = vNewForward.Length();
		pNewTransform->SetScale(Vec3(0.5f, fLength, 1.f));

		Vec3 vNextOffset = vNewForward / 2.f;
		vNextOffset.y = 0.f;
		

		vNewForward.Normalize();
		const Vec3& vForward = pNewTransform->GetUp();
		_float fRadian = acos(vNewForward.Dot(vForward));
		Quaternion qRot = Quaternion::CreateFromAxisAngle(Vec3::UnitY, fRadian);
		pNewTransform->Rotate(qRot);
		pNewTransform->Translate(vNextOffset + m_vecTargets[i]->GetTransform()->GetPosition());
		//pNewTransform->GetRight();


		/*pNewTransform->SetForward(vNewForward);
		Quaternion qRot = Quaternion::CreateFromAxisAngle(Vec3::UnitZ, XMConvertToRadians(90.f));
		pNewTransform->Rotate(qRot);*/
		//pNewTransform->SetRight(vNewForward);

		Attack(pAmmo, m_vecTargets[i + 1], fTimeDelta);
	}
}

void CStrife_Ammo_Static::LateTick(const _float& fTimeDelta)
{
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	if (m_IsChain)
		return;
	if (m_vecTargets.empty())
		return;

	m_vecTargets.clear();

	Super::LateTick(fTimeDelta);
}

void CStrife_Ammo_Static::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_Ammo_Static::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Static::Ready_FixedComponents()
{
 	if (FAILED(Super::Ready_FixedComponents()))
		return E_FAIL;
	GetShader()->SetPassIndex(2);

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_Strife_Ammo_Static"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Static::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Static::Bind_ShaderResources()
{
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	//Color color(DirectX::Colors::Magenta);
	Color color(0.7f, 1.f, 0.7f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_Color", &color, sizeof(Color))))
		return E_FAIL;

	m_UVoffset.y += 0.01f;
	if (m_UVoffset.y > 1.f)
		m_UVoffset.y -= 1.f;
	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &m_UVoffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", 0)))
		return E_FAIL;

	/*if (4 > m_iLightningSwitch)
	{
		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_TextureEx1", m_tProps.iKeyFrame + 1)))
			return E_FAIL;

		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_TextureEx2", m_tProps.iKeyFrame + 5)))
			return E_FAIL;
	}

	++m_iLightningSwitch;

	if (8 == m_iLightningSwitch)
		m_iLightningSwitch = 0;*/

	return S_OK;
}

void CStrife_Ammo_Static::FindTargets()
{
	CLayer*& pLayerUnit = m_pGameInstance->GetCurrentLevelLayers()[LAYERTAG::UNIT_GROUND];
	vector<CGameObject*>& vecUnits = pLayerUnit->GetGameObjects();

	if (vecUnits.empty())
		return;

	const Vec3& vPosition = GetTransform()->GetPosition();

	_float fDistSqr = FLT_MAX;
	CGameObject* pTarget = nullptr;

	for (auto& iter : vecUnits)
	{
		const Vec3& targetPos = iter->GetTransform()->GetPosition();
		Vec3 vDistance = targetPos - vPosition;
		vDistance.y = 0.f;
		_float fCache = vDistance.LengthSquared();

		if (100.f < fCache)
			continue;

		if (0.523599f < acos(m_tProps.vVelocity.Dot(XMVector3Normalize(vDistance))))	// 0.261799f == 15도 // 0.523599f == 30도	// vVelocity는 normalize되어 있어야함.
			continue;

		if (fCache < fDistSqr)
		{
			fDistSqr = fCache;
			pTarget = iter;
		}
	}

	if (nullptr == pTarget)
		return;

	m_vecTargets.push_back(pTarget);
	//
	for (_int i = 0; i < 3; i++)
	{
		CGameObject*& pChainer = m_vecTargets.back();
		pTarget = nullptr;
		fDistSqr = FLT_MAX;

		for (auto& iter : vecUnits)
		{
			_bool bAlreadyTarget = false;
			for (auto& _iter : m_vecTargets)
				if (_iter == iter)
					bAlreadyTarget = true;

			if (bAlreadyTarget)
				continue;

			const Vec3& targetPos = iter->GetTransform()->GetPosition();
			Vec3 vDistance = targetPos - iter->GetTransform()->GetPosition();

			_float fCache = vDistance.LengthSquared();

			if (100.f < fCache)
				continue;

			if (fCache < fDistSqr)
			{
				fDistSqr = fCache;
				pTarget = iter;
			}
		}

		if (nullptr == pTarget)
			return;

		m_vecTargets.push_back(pTarget);
	}
}

void CStrife_Ammo_Static::Attack(CStrife_Ammo_Static* pAmmo, CGameObject* pTarget, const _float& fTimeDelta)
{
	CMonsterController* pMonsterController = static_cast<CMonsterController*>(pTarget->GetScripts()[0]);
	
	m_fFR_Default_Timer -= fTimeDelta;
	if (m_fFR_Default_Timer < 0.f)
	{
		//pMonsterController->GetHitMessage(m_tProps.iDamage);
		pMonsterController->GetHitMessage(1);
		m_fFR_Default_Timer = m_fFR_Default;
	}
}

CStrife_Ammo_Static* CStrife_Ammo_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_Ammo_Static* pInstance = new CStrife_Ammo_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_Ammo_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_Ammo_Static::Clone(void* pArg)
{
	CStrife_Ammo_Static* pInstance = new CStrife_Ammo_Static(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_Ammo_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_Ammo_Static::Free()
{
	Super::Free();
}
