#include "ComponentManager.h"
#include "GameObject.h"
#include "LevelManager.h"

CRigidStatic::CRigidStatic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, RigidBodyType::STATIC)
	, m_fFrictionCoef(0.5f)
{
}

CRigidStatic::CRigidStatic(const CRigidStatic& rhs)
	: Super(rhs)
	, m_fFrictionCoef(rhs.m_fFrictionCoef)
{
}

HRESULT CRigidStatic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidStatic::Initialize(void* pArg)
{
	return S_OK;
}

void CRigidStatic::Tick(_float fTimeDelta)
{
	// ColliderUpdate
	if (3/*LEVEL_GAMETOOL*/ == CLevelManager::GetInstance()->GetCurrentLevelIndex())
	{
		m_pSphereCollider->Tick(fTimeDelta);
		m_pOBBCollider->Tick(fTimeDelta);
	}
}

void CRigidStatic::LateTick(_float fTimeDelta)
{
}

void CRigidStatic::DebugRender()
{
	Super::DebugRender();
}

void CRigidStatic::OnCollisionEnter(const COLLISION_DESC& desc)
{
	// friction
}

void CRigidStatic::OnCollisionStay(const COLLISION_DESC& desc)
{
}

void CRigidStatic::OnCollisionExit(const COLLISION_DESC& desc)
{
}


CRigidStatic* CRigidStatic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidStatic* pInstance = new CRigidStatic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidStatic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidStatic::Clone(CGameObject* pGameObject, void* pArg)
{
	CRigidStatic* pInstance = new CRigidStatic(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRigidStatic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRigidStatic::Free()
{
	Super::Free();
}