#include "stdafx.h"
#include "..\Public\Strife_Ammo_Default.h"
#include "GameInstance.h"
#include "MonsterController.h"
#include "PlayerController.h"

CStrife_Ammo_Default::CStrife_Ammo_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_Ammo_Default::CStrife_Ammo_Default(const CStrife_Ammo_Default& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_Ammo_Default::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Default::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	static_cast<CRigidDynamic*>(GetRigidBody())->UseGravity(false);
	static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(true);
	static_cast<CRigidDynamic*>(GetRigidBody())->SetMass(0.1f);

	GetTransform()->SetScale(Vec3(0.33f, 7.f, 1.f));
	GetTransform()->Rotate(Vec3(90.f, 0.f, 0.f));

	GetRigidBody()->GetSphereCollider()->SetRadius(0.37f);

	Vec3 vExtents(0.25f, 0.25f, 0.25f);
	GetRigidBody()->GetOBBCollider()->SetExtents(vExtents);

	return S_OK;
}

void CStrife_Ammo_Default::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	
	if (!LifeTime(fTimeDelta))
		return;

	Move(fTimeDelta);

	const Vec3& vPos = GetTransform()->GetPosition();

	GetRigidBody()->GetSphereCollider()->SetCenter(vPos);
	GetRigidBody()->GetOBBCollider()->SetCenter(vPos);
}

void CStrife_Ammo_Default::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CStrife_Ammo_Default::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_Ammo_Default::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Default::Ready_FixedComponents()
{
 	if (FAILED(Super::Ready_FixedComponents()))
		return E_FAIL;

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_Strife_Muzzle_Default"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Default::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Default::Bind_ShaderResources()
{
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_Color", &m_vColor, sizeof(Color))))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", m_tProps.iKeyFrame)))
		return E_FAIL;

	return S_OK;
}

void CStrife_Ammo_Default::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::UNIT_GROUND == eLayerTag)
	{
		CMonsterController* pMonsterController = static_cast<CMonsterController*>(pOther->GetScripts()[0]);
		pMonsterController->GetHitMessage(m_tProps.iDamage);

		// 사이클 때문에 어쩔 수 없이 여기서 Kinematic 꺼줘야 함
		static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(false);
	}
	else if (LAYERTAG::PLAYER == eLayerTag)
	{
		CPlayerController* pPlayerController = static_cast<CPlayerController*>(pOther->GetScripts()[0]);
		pPlayerController->GetHitMessage();

		static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(false);
	}

	m_pGameInstance->DeleteObject(this);
}

void CStrife_Ammo_Default::OnCollisionStay(CGameObject* pOther)
{
}

void CStrife_Ammo_Default::OnCollisionExit(CGameObject* pOther)
{
}

CStrife_Ammo_Default* CStrife_Ammo_Default::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_Ammo_Default* pInstance = new CStrife_Ammo_Default(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_Ammo_Default");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_Ammo_Default::Clone(void* pArg)
{
	CStrife_Ammo_Default* pInstance = new CStrife_Ammo_Default(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_Ammo_Default");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_Ammo_Default::Free()
{
	Super::Free();
}
