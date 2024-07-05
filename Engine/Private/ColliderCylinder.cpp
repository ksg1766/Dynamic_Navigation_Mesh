#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "ColliderAABB.h"
#include "ColliderOBB.h"
#include "ColliderCylinder.h"

CCylinderCollider::CCylinderCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Super(pDevice, pContext, ColliderType::Cylinder)
{
}

CCylinderCollider::CCylinderCollider(const CCylinderCollider& rhs)
    :Super(rhs)
    , m_tBoundingCylinder(rhs.m_tBoundingCylinder)
{
}

HRESULT CCylinderCollider::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCylinderCollider::Initialize(void* pArg)
{
    return S_OK;
}

void CCylinderCollider::Tick(_float fTimeDelta)
{
}

void CCylinderCollider::LateTick(_float fTimeDelta)
{
}

void CCylinderCollider::DebugRender()
{
}

_bool CCylinderCollider::Intersects(Ray& ray, OUT _float& distance)
{
    return m_tBoundingCylinder.Intersects(ray.position, ray.direction, OUT distance);
}

_bool CCylinderCollider::Intersects(Super* other)
{
    ColliderType type = other->GetColliderType();

    switch (type)
    {
    case ColliderType::Sphere:
        return m_tBoundingCylinder.Intersects(dynamic_cast<CSphereCollider*>(other)->GetBoundingSphere());
    case ColliderType::AABB:
        return m_tBoundingCylinder.Intersects(dynamic_cast<CAABBCollider*>(other)->GetBoundingBox());
    case ColliderType::OBB:
        return m_tBoundingCylinder.Intersects(dynamic_cast<COBBCollider*>(other)->GetBoundingBox());
    case ColliderType::Cylinder:
        return m_tBoundingCylinder.Intersects(dynamic_cast<CCylinderCollider*>(other)->GetBoundingCylinder());
    }

    return false;
}

CCylinderCollider* CCylinderCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCylinderCollider* pInstance = new CCylinderCollider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCylinderCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCylinderCollider::Clone(CGameObject* pGameObject, void* pArg)
{
    CCylinderCollider* pInstance = new CCylinderCollider(*this);
    pInstance->m_pGameObject = pGameObject;

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CCylinderCollider");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCylinderCollider::Free()
{
    Super::Free();
}
