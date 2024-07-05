#pragma once
#include "RigidBodyBase.h"

BEGIN(Engine)

class ENGINE_DLL CRigidStatic :
    public CRigidBody
{
	using Super = CRigidBody;

private:
	CRigidStatic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidStatic(const CRigidStatic& rhs);
	virtual ~CRigidStatic() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
#pragma region Collision
	virtual	void	OnCollisionEnter(const COLLISION_DESC& desc)override;
	virtual	void	OnCollisionStay(const COLLISION_DESC& desc)	override;
	virtual	void	OnCollisionExit(const COLLISION_DESC& desc)	override;
#pragma endregion Collision

private:
	_float	m_fFrictionCoef;

public:
	static CRigidStatic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)		override;
	virtual void Free()							override;
};

END