#pragma once
#include "ColliderBase.h"

BEGIN(Engine)

class ENGINE_DLL CSphereCollider : public CCollider
{
	using Super = CCollider;
private:
	CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()					override;
	virtual HRESULT Initialize(void* pArg)					override;
	virtual void	Tick(const _float& fTimeDelta)			override;
	virtual void	LateTick(const _float& fTimeDelta)		override;
	virtual void	DebugRender()							override;

	virtual _bool	Intersects(Ray& ray, OUT _float& distance)	override;
	virtual _bool	Intersects(Super* other)					override;

	void			SetRadius(const _float& radius)		{ m_tBoundingSphere.Radius = radius; }
	void			SetCenter(const Vec3& vCenter)		{ m_tBoundingSphere.Center = vCenter; }
	BoundingSphere&	GetBoundingSphere()					{ return m_tBoundingSphere; }

	virtual void	OnTriggerEnter(const CCollider* pOther)	override;
	virtual void	OnTriggerStay(const CCollider* pOther)	override;
	virtual void	OnTriggerExit(const CCollider* pOther)	override;

private:
	BoundingSphere	m_tBoundingSphere;

public:
	static	CSphereCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free()						override;
};

END