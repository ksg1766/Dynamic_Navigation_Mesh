#pragma once
#include "Component.h"

BEGIN(Engine)

//class CCollider;
class COBBCollider;
class CSphereCollider;

enum class RigidBodyType
{
	STATIC,
	DYNAMIC
};

class ENGINE_DLL CRigidBody :
    public CComponent
{
	using Super = CComponent;

protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RigidBodyType eRigidBodyType);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT		Initialize_Prototype()				override;
	virtual HRESULT		Initialize(void* pArg)				override;
	//virtual HRESULT	FixedUpdate(const _float& fTimeDelta)	PURE;
	virtual void		Tick(const _float& fTimeDelta)		override;
	virtual void		LateTick(const _float& fTimeDelta)	override;
	virtual void		DebugRender()						override;

	HRESULT				InitializeCollider();

	RigidBodyType&		GetRigidBodyType()		{ return m_eRigidBodyType; }
	CSphereCollider*&	GetSphereCollider()		{ return m_pSphereCollider; }
	COBBCollider*&		GetOBBCollider()		{ return m_pOBBCollider; }
	//CCollider*&		GetBoxCollider()		{ return m_pBoxCollider; }
	/*CCollider*		GetCollider(ColliderType eColliderType) { return m_arrColliderGroup[(_uint)eColliderType]; }*/

public:
	// 충돌 상대가 RigidDynamic && Kinetic일 때 상대의 OnCollision도 호출
	virtual	void		OnCollisionEnter(const COLLISION_DESC& desc)PURE;
	virtual	void		OnCollisionStay(const COLLISION_DESC& desc)	PURE;
	virtual	void		OnCollisionExit(const COLLISION_DESC& desc)	PURE;

protected:
	RigidBodyType		m_eRigidBodyType;
	CSphereCollider*	m_pSphereCollider = nullptr;
	COBBCollider*		m_pOBBCollider = nullptr;	
	//CCollider*		m_pBoxCollider = nullptr;	
	//array<CCollider*, 2>	m_arrColliderGroup;

public:
	virtual void Free()						override;
};

END