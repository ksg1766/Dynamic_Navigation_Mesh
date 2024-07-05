#include "ComponentManager.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RigidBodyType eRigidBodyType)
	: Super(pDevice, pContext, ComponentType::RigidBody)
	, m_eRigidBodyType(eRigidBodyType)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: Super(rhs)
	, m_eRigidBodyType(rhs.m_eRigidBodyType)
{
}

void CRigidBody::Free()
{
	Super::Free();
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	return S_OK;
}

void CRigidBody::Tick(_float fTimeDelta)
{
}

void CRigidBody::LateTick(_float fTimeDelta)
{
}

void CRigidBody::DebugRender()
{
	if (m_pSphereCollider) m_pSphereCollider->DebugRender();
	if (m_pOBBCollider) m_pOBBCollider->DebugRender();
}

HRESULT CRigidBody::InitializeCollider()
{
	m_pSphereCollider = static_cast<CSphereCollider*>(CComponentManager::GetInstance()
		->Clone_Component(m_pGameObject, 0, TEXT("Prototype_Component_SphereCollider"), nullptr));
	m_pOBBCollider = static_cast<COBBCollider*>(CComponentManager::GetInstance()
		->Clone_Component(m_pGameObject, 0, TEXT("Prototype_Component_OBBCollider"), nullptr));

	if (!m_pSphereCollider || !m_pOBBCollider)
		return E_FAIL;

	return S_OK;
}
