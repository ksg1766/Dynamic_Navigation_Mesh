#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "RigidDynamic.h"
#include "Transform.h"
#include "NavMeshAgent.h"

BEGIN(Engine)

class CTransform;
class CNavMeshAgent;

END

BEGIN(Client)

class CMonsterStats;
class CBossController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CBossController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossController(const CBossController& rhs);
	virtual ~CBossController() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;

public:
	void	GetMoveMessage(const Vec3& vDir)		{ m_vNetMove += vDir; }
	void	GetTranslateMessage(const Vec3& vDir)	{ m_vNetTrans += vDir; }
	void	GetDashMessage(const _bool& IsDash)		{ IsDash ? Dash(m_pTransform->GetForward()) : DashEnd(); }
	void	GetAttackMessage(_int iSkillIndex)		{ Attack(iSkillIndex); }
	void	GetHitMessage(_int iDamage);
	void	GetMaxSpeedMessage()					{ m_bMax = true; }

	void	Look(const Vec3& vPoint, const _float& fTimeDelta = 1.f);
	void	ForceHeight()							{ m_pNavMeshAgent->ForceHeight(); }
	_float	GetHeightOffset()						{ return m_pNavMeshAgent->GetHeightOffset(); }
	_bool	Walkable(_fvector vPoint)				{ return m_pNavMeshAgent->Walkable(vPoint); }

	_bool	IsZeroHP()								{ return m_IsZeroHP; }
	CMonsterStats*	GetStats()						{ return m_pStats; }
	void	SetStats(CMonsterStats* pStats)			{ m_pStats = pStats; }

public:
	void	OnCollisionEnter(CGameObject* pOther);
	void	OnCollisionStay(CGameObject* pOther);
	void	OnCollisionExit(CGameObject* pOther);

private:
	void	Move(const _float& fTimeDelta);
	void	Translate(const _float& fTimeDelta);
	void	Dash(const Vec3& vDir);
	void	DashEnd();
	void	Attack(_int iSkillIndex);
	void	Hit(_int iDamage);

	void	LimitAllAxisVelocity();

private:
	CTransform*		m_pTransform	= nullptr;
	CRigidDynamic*	m_pRigidBody	= nullptr;
	CNavMeshAgent*	m_pNavMeshAgent	= nullptr;
	Vec3			m_vPrePos;

	Vec3			m_vNetMove;
	Vec3			m_vNetTrans;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;
	_bool			m_bMax			= false;

	Vec3			m_vMaxAngularSpeed;
	Vec3			m_vAngularSpeed;

	CGameObject*	m_pTarget		= nullptr;
	Vec3			m_vTargetPoint;

	CMonsterStats*	m_pStats		= nullptr;
	_bool			m_IsZeroHP		= false;
	_int			m_iHitEffectCount = -1;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect			= nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
#endif

public:
	static	CBossController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END