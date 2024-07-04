#pragma once
#include "Component.h"

BEGIN(Engine)

enum class ColliderType
{
	Sphere,
	AABB,
	OBB,
	Cylinder
};

class CRigidBody;

class ENGINE_DLL CCollider : public CComponent
{
	using Super = CComponent;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ColliderType eColliderType);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()						override;
	virtual HRESULT Initialize(void* pArg)						override;
	virtual void	DebugRender()								override;


	virtual _bool	Intersects(Ray& ray, OUT float& distance)	PURE;
	virtual _bool	Intersects(CCollider* other)				PURE;

	ColliderType	GetColliderType()			{ return m_eColliderType; }
	_uint			GetID()						{ return m_iID; }
	CRigidBody*		GetRigidBody()				{ return m_pRigidBody; }

	_bool			IsTrigger()					{ return m_IsTrigger; }
	void			SetTrigger(_bool IsTrigger) { m_IsTrigger = IsTrigger; }

	// 충돌 대상이 RigidStatic || (RigidDynamic && Kinematic)일 때
	// 총알 vs 몬스터 충돌 시 총알의 Collision, 몬스터의 Trigger 호출.
	// 몬스터의 HP <= 0 일 때, 총알의 Collision에서 상대의 AddForce호출로 날려버리기(좀 더 고민해보자)
	virtual	void	OnTriggerEnter(const CCollider* pOther){};
	virtual	void	OnTriggerStay(const CCollider* pOther) {};
	virtual	void	OnTriggerExit(const CCollider* pOther) {};

protected:
	CRigidBody*		m_pRigidBody;

private:
	ColliderType	m_eColliderType;
	_bool			m_IsTrigger;

	_uint			m_iID;
	static _uint	g_iNextID;

	//_int			m_iCol;

#ifdef _DEBUG
//protected:
//	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
//	BasicEffect*							m_pEffect = nullptr;
//	ID3D11InputLayout*						m_pInputLayout = nullptr;
#endif

public:
	virtual void Free()						override;
};

END