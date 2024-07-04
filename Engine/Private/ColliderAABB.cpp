#include "Engine_Defines.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "DebugDraw.h"

CAABBCollider::CAABBCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext, ColliderType::AABB)
{
}

CAABBCollider::CAABBCollider(const CAABBCollider& rhs)
	:Super(rhs)
	, m_tBoundingBox(rhs.m_tBoundingBox)
{
}

HRESULT CAABBCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAABBCollider::Initialize(void* pArg)
{
	return S_OK;
}

void CAABBCollider::Tick(const _float& fTimeDelta)
{
	CTransform* pTransform = GetGameObject()->GetTransform();

	m_tBoundingBox.Center = pTransform->GetPosition();
}

void CAABBCollider::LateTick(const _float& fTimeDelta)
{
}

void CAABBCollider::DebugRender()
{
#ifdef _DEBUG
	//Super::DebugRender();

	//m_pBatch->Begin();

	//DX::Draw(m_pBatch, m_tBoundingBox, Colors::Lime);

	//m_pBatch->End();
#endif // DEBUG
}

_bool CAABBCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return m_tBoundingBox.Intersects(ray.position, ray.direction, OUT distance);
}

_bool CAABBCollider::Intersects(Super* other)
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

CAABBCollider* CAABBCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAABBCollider* pInstance = new CAABBCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAABBCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAABBCollider::Clone(CGameObject* pGameObject, void* pArg)
{
	CAABBCollider* pInstance = new CAABBCollider(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAABBCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAABBCollider::Free()
{
	Super::Free();
}
