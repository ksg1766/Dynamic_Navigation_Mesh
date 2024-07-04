#include "Engine_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "ColliderSphere.h"
#include "ColliderAABB.h"
#include "ColliderOBB.h"
#include "ColliderCylinder.h"
#include "DebugDraw.h"

COBBCollider::COBBCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, ColliderType::OBB)
{

}

COBBCollider::COBBCollider(const COBBCollider& rhs)
	: Super(rhs)
	, m_tBoundingBox(rhs.m_tBoundingBox)
{
}

HRESULT COBBCollider::Initialize_Prototype()
{
	Super::Initialize_Prototype();

	return S_OK;
}

HRESULT COBBCollider::Initialize(void* pArg)
{
	//Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	//m_tBoundingBox.Extents = scale;
	//m_pRigidBody = m_pGameObject->GetRigidBody();
	CTransform* pTransform = GetGameObject()->GetTransform();

	m_tBoundingBox.Center = pTransform->GetPosition();
	m_tBoundingBox.Extents = 0.5f * pTransform->GetLocalScale();
	m_tBoundingBox.Orientation = pTransform->GetRotationQuaternion();



	return S_OK;
}

void COBBCollider::Tick(const _float& fTimeDelta)
{
	CTransform* pTransform = GetGameObject()->GetTransform();

	m_tBoundingBox.Center = pTransform->GetPosition() + _float3(0.f, m_tBoundingBox.Extents.y, 0.f);
	m_tBoundingBox.Orientation = pTransform->GetRotationQuaternion();
}

void COBBCollider::LateTick(const _float& fTimeDelta)
{
}

void COBBCollider::DebugRender()
{
#ifdef _DEBUG
	//Super::DebugRender();

	//m_pBatch->Begin();

	//DX::Draw(m_pBatch, m_tBoundingBox, Colors::Lime);

	//m_pBatch->End();
#endif // DEBUG
}

_bool COBBCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return m_tBoundingBox.Intersects(ray.position, ray.direction, OUT distance);
}

_bool COBBCollider::Intersects(Super* other)
{
	ColliderType type = other->GetColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return m_tBoundingBox.Intersects(static_cast<CSphereCollider*>(other)->GetBoundingSphere());
	case ColliderType::AABB:
		return m_tBoundingBox.Intersects(static_cast<CAABBCollider*>(other)->GetBoundingBox());
	case ColliderType::OBB:
		return m_tBoundingBox.Intersects(static_cast<COBBCollider*>(other)->GetBoundingBox());
	//case ColliderType::Cylinder:
	//	return m_tBoundingBox.Intersects(static_cast<CCylinderCollider*>(other)->GetBoundingCylinder());
	}

	return false;
}

COBBCollider* COBBCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COBBCollider* pInstance = new COBBCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : COBBCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* COBBCollider::Clone(CGameObject* pGameObject, void* pArg)
{
	COBBCollider* pInstance = new COBBCollider(*this);
	pInstance->m_pGameObject = pGameObject;
	pInstance->m_pRigidBody = pInstance->m_pGameObject->GetRigidBody();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : COBBCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COBBCollider::Free()
{
	Super::Free();
}
