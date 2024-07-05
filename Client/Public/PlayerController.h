#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "RigidDynamic.h"
#include "NavMeshAgent.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CPlayerController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CPlayerController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerController(const CPlayerController& rhs);
	virtual ~CPlayerController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
	_bool	IsIdle();
	_bool	IsRun();
	_bool	IsAim();
	_bool	IsJump();
	_bool	IsDash();

	void	GetMoveMessage(const Vec3& vDir)		{ m_vNetMove += vDir; }
	void	GetTranslateMessage(const Vec3& vDir)	{ m_vNetTrans += vDir; }
	void	GetJumpMessage(_bool IsJump)			{ IsJump ? Jump() : Land();}
	void	GetDashMessage(_bool IsDash)			{ IsDash ? Dash(m_pTransform->GetForward()) : DashEnd(); }
	void	GetHitMessage()							{ Hit(); }

	void	ForceHeight()				{ m_pNavMeshAgent->ForceHeight(); }
	_float	GetHeightOffset()			{ return m_pNavMeshAgent->GetHeightOffset(); }
	_bool	Walkable(_fvector vPoint)	{ return m_pNavMeshAgent->Walkable(vPoint); }


	_bool	Pick(_uint screenX, _uint screenY, Vec3& pickPos, _float& distance);
	void	Look(const Vec3& vPoint, _float fTimeDelta);

public:
	void	OnCollisionEnter(CGameObject* pOther);
	void	OnCollisionStay(CGameObject* pOther);
	void	OnCollisionExit(CGameObject* pOther);

private:
	void	Input(_float fTimeDelta);
	void	Move(_float fTimeDelta);
	void	Translate(_float fTimeDelta);
	//void	Look(const Vec3& vPoint);
	void	Jump();
	void	Land();
	void	Dash(const Vec3& vDir);
	void	DashEnd();
	void	Hit();

	void	LimitAllAxisVelocity();

private:
	CTransform*		m_pTransform = nullptr;
	CRigidDynamic*	m_pRigidBody = nullptr;
	CNavMeshAgent*	m_pNavMeshAgent = nullptr;
	Vec3			m_vPrePos;

	Vec3			m_vNetMove;
	Vec3			m_vNetTrans;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	Vec3			m_vMaxAngularSpeed;
	Vec3			m_vAngularSpeed;

	_bool			m_bFireLR = true;

	_bool			m_bDagonCreated = false;
	_int			m_bGodRayScene = -1;
	_bool			m_bMolochCreated = false;
	_bool			m_bWaterCreated = false;

	_int			m_iHitEffectCount = -1;

public:
	static	CPlayerController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END