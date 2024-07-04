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

	// �浹 ����� RigidStatic || (RigidDynamic && Kinematic)�� ��
	// �Ѿ� vs ���� �浹 �� �Ѿ��� Collision, ������ Trigger ȣ��.
	// ������ HP <= 0 �� ��, �Ѿ��� Collision���� ����� AddForceȣ��� ����������(�� �� ����غ���)
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