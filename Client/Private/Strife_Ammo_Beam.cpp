#include "stdafx.h"
#include "..\Public\Strife_Ammo_Beam.h"
#include "GameInstance.h"
#include "Layer.h"
#include "MonsterController.h"
#include "Strife_Ammo_Beam.h"

_float2	CStrife_Ammo_Beam::m_UVoffset = _float2(0.f, 0.f);
_float	CStrife_Ammo_Beam::m_fFR_Default = 0.2f;
_float	CStrife_Ammo_Beam::m_fFR_Default_Timer = 0.f;

CStrife_Ammo_Beam::CStrife_Ammo_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_Ammo_Beam::CStrife_Ammo_Beam(const CStrife_Ammo_Beam& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_Ammo_Beam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Beam::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetTransform()->SetScale(Vec3(1.2f, 20.f, 1.f));
	GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(true);
	static_cast<CRigidDynamic*>(GetRigidBody())->UseGravity(false);

	return S_OK;
}

void CStrife_Ammo_Beam::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	m_UVoffset.y += fTimeDelta;
	
	m_fFR_Default_Timer -= fTimeDelta;

	Vec3 vExtents(0.6f, 11.f, 0.5f);
	const Vec3& vPos = GetTransform()->GetPosition();
	GetRigidBody()->GetSphereCollider()->SetRadius(11.f);
	GetRigidBody()->GetOBBCollider()->SetCenter(vPos);
	GetRigidBody()->GetOBBCollider()->SetExtents(vExtents);

	m_pGameInstance->DeleteObject(this);
}

void CStrife_Ammo_Beam::LateTick(const _float& fTimeDelta)
{
	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);

	Super::LateTick(fTimeDelta);
}

void CStrife_Ammo_Beam::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_Ammo_Beam::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Beam::Ready_FixedComponents()
{
 	if (FAILED(Super::Ready_FixedComponents()))
		return E_FAIL;
	GetShader()->SetPassIndex(2);

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_Strife_Ammo_Beam"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Beam::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Beam::Bind_ShaderResources()
{
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	if (m_UVoffset.y > 1.f)
		m_UVoffset.y -= 1.f;
	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &m_UVoffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResources(GetShader(), "g_Textures")))
		return E_FAIL;

	return S_OK;
}

void CStrife_Ammo_Beam::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::UNIT_GROUND == eLayerTag)
	{
		if (m_fFR_Default_Timer < 0.f)
		{
			m_fFR_Default_Timer = m_fFR_Default;

			CMonsterController* pMonsterController = static_cast<CMonsterController*>(pOther->GetScripts()[0]);
			pMonsterController->GetHitMessage(m_tProps.iDamage);
		}
	}
}

void CStrife_Ammo_Beam::OnCollisionStay(CGameObject* pOther)
{

}

void CStrife_Ammo_Beam::OnCollisionExit(CGameObject* pOther)
{

}

CStrife_Ammo_Beam* CStrife_Ammo_Beam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_Ammo_Beam* pInstance = new CStrife_Ammo_Beam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_Ammo_Beam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_Ammo_Beam::Clone(void* pArg)
{
	CStrife_Ammo_Beam* pInstance = new CStrife_Ammo_Beam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_Ammo_Beam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_Ammo_Beam::Free()
{
	Super::Free();
}
