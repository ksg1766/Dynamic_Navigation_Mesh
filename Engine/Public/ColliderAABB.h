#pragma once
#include "ColliderBase.h"

BEGIN(Engine)

class ENGINE_DLL CAABBCollider : public CCollider
{
	using Super = CCollider;
private:
	CAABBCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAABBCollider(const CAABBCollider& rhs);
	virtual ~CAABBCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()					override;
	virtual HRESULT Initialize(void* pArg)					override;
	virtual void	Tick(const _float& fTimeDelta)			override;
	virtual void	LateTick(const _float& fTimeDelta)		override;
	virtual void	DebugRender()							override;

	virtual _bool	Intersects(Ray& ray, OUT _float& distance)	override;
	virtual _bool	Intersects(Super* other)					override;

	void			SetExtents(const Vec3& vExtents)	{ m_tBoundingBox.Extents = vExtents; }
	void			SetCenter(const Vec3& vCenter)		{ m_tBoundingBox.Center = vCenter; }
	BoundingBox&	GetBoundingBox()					{ return m_tBoundingBox; }

private:
	BoundingBox m_tBoundingBox;

public:
	static CAABBCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free()						override;
};

END