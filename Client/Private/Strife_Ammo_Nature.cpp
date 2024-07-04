#include "stdafx.h"
#include "..\Public\Strife_Ammo_Nature.h"
#include "GameInstance.h"
#include "MonsterController.h"

CStrife_Ammo_Nature::CStrife_Ammo_Nature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_Ammo_Nature::CStrife_Ammo_Nature(const CStrife_Ammo_Nature& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_Ammo_Nature::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Nature::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	static_cast<CRigidDynamic*>(GetRigidBody())->UseGravity(false);
	static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(true);
	static_cast<CRigidDynamic*>(GetRigidBody())->SetMass(0.05f);

	GetTransform()->SetScale(Vec3(0.33f, 5.5f, 1.f));
	GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	GetRigidBody()->GetSphereCollider()->SetRadius(0.5f);

	Vec3 vExtents(0.25f, 0.25f, 0.25f);
	GetRigidBody()->GetOBBCollider()->SetExtents(vExtents);

	return S_OK;
}

void CStrife_Ammo_Nature::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	
	if (!LifeTime(fTimeDelta))
		return;

	Move(fTimeDelta);

	const Vec3& vPos = GetTransform()->GetPosition();

	GetRigidBody()->GetSphereCollider()->SetCenter(vPos);
	GetRigidBody()->GetOBBCollider()->SetCenter(vPos);
}

void CStrife_Ammo_Nature::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CStrife_Ammo_Nature::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_Ammo_Nature::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo_Nature::Ready_FixedComponents()
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

HRESULT CStrife_Ammo_Nature::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_Ammo_Nature::Bind_ShaderResources()
{
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	//Color color(0.85f, 0.57f, 0.24f, 1.f);
	Color color(0.52f, 0.971960814f, 0.52f, 1.f);
	//Color color(DirectX::Colors::Green);
	//Color color(DirectX::Colors::GreenYellow);
	if (FAILED(GetShader()->Bind_RawValue("g_Color", &color, sizeof(Color))))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", m_tProps.iKeyFrame)))
		return E_FAIL;

	return S_OK;
}

void CStrife_Ammo_Nature::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::UNIT_GROUND == eLayerTag)
	{
		CMonsterController* pMonsterController = static_cast<CMonsterController*>(pOther->GetScripts()[0]);
		pMonsterController->GetHitMessage(m_tProps.iDamage);

		// 사이클 때문에 어쩔 수 없이 여기서 Kinematic 꺼줘야 함
		static_cast<CRigidDynamic*>(GetRigidBody())->IsKinematic(false);
	}

	m_pGameInstance->DeleteObject(this);
}

void CStrife_Ammo_Nature::OnCollisionStay(CGameObject* pOther)
{
}

void CStrife_Ammo_Nature::OnCollisionExit(CGameObject* pOther)
{
}

CStrife_Ammo_Nature* CStrife_Ammo_Nature::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_Ammo_Nature* pInstance = new CStrife_Ammo_Nature(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_Ammo_Nature");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_Ammo_Nature::Clone(void* pArg)
{
	CStrife_Ammo_Nature* pInstance = new CStrife_Ammo_Nature(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_Ammo_Nature");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_Ammo_Nature::Free()
{
	Super::Free();
}
