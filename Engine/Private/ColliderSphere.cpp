#include "Engine_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "ColliderSphere.h"
#include "ColliderAABB.h"
#include "ColliderOBB.h"
#include "ColliderCylinder.h"
#include "RigidDynamic.h"
#include "DebugDraw.h"

CSphereCollider::CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext, ColliderType::Sphere)
{
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
	:Super(rhs)
	, m_tBoundingSphere(rhs.m_tBoundingSphere)
{

}

HRESULT CSphereCollider::Initialize_Prototype()
{
	Super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSphereCollider::Initialize(void* pArg)
{
	//m_pRigidBody = m_pGameObject->GetRigidBody();
	CTransform* pTransform = GetGameObject()->GetTransform();

	m_tBoundingSphere.Center = pTransform->GetPosition();
	m_tBoundingSphere.Radius = pTransform->GetLocalScale().Length() / 2.f;

	return S_OK;
}

void CSphereCollider::Tick(_float fTimeDelta)
{
	CTransform* pTransform = GetGameObject()->GetTransform();

	m_tBoundingSphere.Center = pTransform->GetPosition() + _float3(0.f, m_tBoundingSphere.Radius, 0.f);
	//m_tBoundingSphere.Radius = pTransform->GetLocalScale().Length() / 2.f;

	//Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	//m_tBoundingSphere.Radius = m_fRadius * max(max(scale.x, scale.y), scale.z);
}

void CSphereCollider::LateTick(_float fTimeDelta)
{
}

void CSphereCollider::DebugRender()
{
#ifdef _DEBUG
	//Super::DebugRender();

	//m_pBatch->Begin();

	//DX::Draw(m_pBatch, m_tBoundingSphere, Colors::Green);

	//m_pBatch->End();
#endif // DEBUG
}
_bool CSphereCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return m_tBoundingSphere.Intersects(ray.position, ray.direction, OUT distance);
}

_bool CSphereCollider::Intersects(Super* other)
{
	ColliderType type = other->GetColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return m_tBoundingSphere.Intersects(static_cast<CSphereCollider*>(other)->GetBoundingSphere());
	case ColliderType::AABB:
		return m_tBoundingSphere.Intersects(static_cast<CAABBCollider*>(other)->GetBoundingBox());
	case ColliderType::OBB:
		return m_tBoundingSphere.Intersects(static_cast<COBBCollider*>(other)->GetBoundingBox());
	//case ColliderType::Cylinder:
	//	return m_tBoundingSphere.Intersects(static_cast<CCylinderCollider*>(other)->GetBoundingCylinder());
	}

	return false;
}

CSphereCollider* CSphereCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphereCollider* pInstance = new CSphereCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSphereCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSphereCollider::Clone(CGameObject* pGameObject, void* pArg)
{
	CSphereCollider* pInstance = new CSphereCollider(*this);
	pInstance->m_pGameObject = pGameObject;
	pInstance->m_pRigidBody = pInstance->m_pGameObject->GetRigidBody();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSphereCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSphereCollider::Free()
{
	Super::Free();
}

void CSphereCollider::OnTriggerEnter(const CCollider* pOther)
{
	m_pGameObject->OnTriggerEnter(pOther->GetGameObject());
}

void CSphereCollider::OnTriggerStay(const CCollider* pOther)
{
	m_pGameObject->OnTriggerStay(pOther->GetGameObject());
}

void CSphereCollider::OnTriggerExit(const CCollider* pOther)
{
	m_pGameObject->OnTriggerExit(pOther->GetGameObject());
}
