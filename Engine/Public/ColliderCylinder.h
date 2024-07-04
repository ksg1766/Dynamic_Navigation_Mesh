#pragma once
#include "ColliderBase.h"

BEGIN(Engine)

struct BoundingCylinder
{
	_float3 Center;
	_float	Radius;
	_float	YExtents;

	BoundingCylinder() noexcept : Center(0, 0, 0), Radius(1.f), YExtents(1.f) {}

	BoundingCylinder(const BoundingCylinder&) = default;
	BoundingCylinder& operator=(const BoundingCylinder&) = default;

	constexpr BoundingCylinder(_In_ const XMFLOAT3& center, _In_ _float radius, _In_ _float yExtents) noexcept
		: Center(center), Radius(radius), YExtents(yExtents) {}

	// Methods
	void    XM_CALLCONV     Transform(_Out_ BoundingCylinder& Out, _In_ FXMMATRIX M) const noexcept;
	void    XM_CALLCONV     Transform(_Out_ BoundingCylinder& Out, _In_ float Scale, _In_ FXMVECTOR Rotation, _In_ FXMVECTOR Translation) const noexcept;

	_bool XM_CALLCONV Intersects(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) const noexcept
	{
		assert(DirectX::Internal::XMVector3IsUnit(Direction));
		//
		return false;
	}

	_bool Intersects(const BoundingSphere& sh)
	{
		//
		return false;
	}
	_bool Intersects(const BoundingBox& sh)
	{
		//
		return false;
	}
	_bool Intersects(const BoundingOrientedBox& sh)
	{
		//
		return false;
	}
	_bool Intersects(const BoundingCylinder& sh)
	{
		//
		return false;
	}
};

class ENGINE_DLL CCylinderCollider :
    public CCollider
{
    using Super = CCollider;
private:
	CCylinderCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCylinderCollider(const CCylinderCollider& rhs);
	virtual ~CCylinderCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()					override;
	virtual HRESULT Initialize(void* pArg)					override;
	virtual void	Tick(const _float& fTimeDelta)			override;
	virtual void	LateTick(const _float& fTimeDelta)		override;
	virtual void	DebugRender()							override;

	virtual _bool	Intersects(Ray& ray, OUT _float& distance)	override;
	virtual _bool	Intersects(Super* other)					override;

	BoundingCylinder& GetBoundingCylinder() { return m_tBoundingCylinder; }

private:
	BoundingCylinder m_tBoundingCylinder;

public:
	static CCylinderCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free()						override;
};

END