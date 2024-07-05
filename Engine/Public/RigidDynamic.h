#pragma once
#include "RigidBodyBase.h"

BEGIN(Engine)

enum class ForceMode
{
	FORCE,
	IMPULSE,
	VELOCITY_CHANGE,
	ACCELERATION
};

class ENGINE_DLL CRigidDynamic :
    public CRigidBody
{
	using Super = CRigidBody;

private:
	CRigidDynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidDynamic(const CRigidDynamic& rhs);
	virtual ~CRigidDynamic() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:

#pragma region Property
	_bool	UseGravity()	const							{ return m_UseGravity; }
	void	UseGravity(_bool UseGravity)					{ m_UseGravity = UseGravity; }
	_bool	IsKinematic()	const							{ return m_IsKinematic; }
	void	IsKinematic(_bool IsKinematic)					{ m_IsKinematic = IsKinematic; }

	_float	GetMass()	const								{ return m_fMass; }
	void	SetMass(_float fMass)							{ m_fMass = fMass; }
	_float	GetDrag()	const								{ return m_fDrag; }
	_float	GetAngularDrag()	const						{ return m_fAngularDrag; }
	void	SetMaterialDrag(_float fMaterialDrag)			{ m_fMaterialDrag = fMaterialDrag; }
	void	SetMaterialAngularDrag(_float fMaterialAngularDrag)	
															{ m_fMaterialAngularDrag = fMaterialAngularDrag; }

	_bool	IsFrozePosition(Axis eAxis)						{ return m_byConstraints & 1 << (_int)eAxis; }
	void	FreezePosition(Axis eAxis)						{ m_byConstraints ^= 1 << (_int)eAxis; }		// Switch On/Off
	_bool	IsFrozeRotation(Axis eAxis)						{ return m_byConstraints & 1 << ((_int)eAxis + 3); }
	void	FreezeRotation(Axis eAxis)						{ m_byConstraints ^= 1 << ((_int)eAxis + 3); }	// Switch On/Off

	// Velocity
	Vec3	GetLinearVelocity()	const						{ return m_vLinearVelocity; }
	void	SetLinearVelocity(const Vec3& vLinearVelocity);
	_float	GetLinearAxisVelocity(Axis eAxis) const	{ return *((_float*)&m_vLinearVelocity + (_int)eAxis); }
	void	SetLinearAxisVelocity(Axis eAxis, _float fVelocity)
															{ *((_float*)&m_vLinearVelocity + (_int)eAxis) = fVelocity; }

	Vec3	GetAngularVelocity()							{ return m_vAngularVelocity; }
	void	SetAngularVelocity(const Vec3& vAngularVelocity);
	_float	GetAngularAxisVelocity(Axis eAxis) const	{ return *((_float*)&m_vAngularVelocity + (_int)eAxis); }
	void	SetAngularAxisVelocity(Axis eAxis, _float fVelocity)
															{ *((_float*)&m_vAngularVelocity + (_int)eAxis) = fVelocity; }

	// Force/Torque modifiers
	void	AddForce(const Vec3& vForce, ForceMode eMode);
	void	AddTorque(const Vec3& vTorque, ForceMode eMode);
	void	SetForceAndTorque(const Vec3& vForce, const Vec3& vTorque, ForceMode eMode);

	void	ClearForce(ForceMode eMode);
	void	ClearTorque(ForceMode eMode);
	void	ClearNetPower();
#pragma endregion Property

#pragma region Collision
	virtual	void	OnCollisionEnter(const COLLISION_DESC& desc)override;
	virtual	void	OnCollisionStay(const COLLISION_DESC& desc)	override;
	virtual	void	OnCollisionExit(const COLLISION_DESC& desc)	override;
#pragma endregion Collision

private:
	void	KineticUpdate(_float fTimeDelta);
	void	KinematicUpdate(_float fTimeDelta);

	void	UpdateTransform(_float fTimeDelta);

	void	Sleep()		{ ClearNetPower(); m_IsSleeping = true;	}
	void	WakeUp()	{ m_IsSleeping = false; }

private:
	_bool	m_IsSleeping;
	const _float m_fSleepThreshold;

	// Description
	_bool	m_UseGravity;
	_bool	m_IsKinematic;

	_float	m_fMass;
	_float	m_fMaterialDrag;
	_float	m_fMaterialAngularDrag;
	_float	m_fDrag;
	_float	m_fAngularDrag;

	// Constraints
	_byte	m_byConstraints;

	Vec3	m_vLinearAcceleration;
	Vec3	m_vAngularAcceleration;

	Vec3	m_vLinearVelocity;
	Vec3	m_vAngularVelocity;

	//Vec3	m_vPrePosition;

public:
	static CRigidDynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free()						override;
};

END