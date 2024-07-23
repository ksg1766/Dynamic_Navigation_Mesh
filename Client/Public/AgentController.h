#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "NavMeshAgent.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CAgentController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAgentController(const CAgentController& rhs);
	virtual ~CAgentController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
	_bool	IsIdle();
	_bool	IsMoving();

	void	ForceHeight()				{ m_pNavMeshAgent->ForceHeight(); }
	_float	GetHeightOffset()			{ return m_pNavMeshAgent->GetHeightOffset(); }
	_bool	Walkable(_fvector vPoint)	{ return m_pNavMeshAgent->Walkable(vPoint); }

	_bool	Pick(_uint screenX, _uint screenY, OUT Vec3& pickPos, OUT _float& distance);

private:
	void	Input(_float fTimeDelta);
	void	Move(_float fTimeDelta);

private:
	CTransform*		m_pTransform = nullptr;
	CNavMeshAgent*	m_pNavMeshAgent = nullptr;
	Vec3			m_vPrePos;

	Vec3			m_vNetMove;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

public:
	static	CAgentController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END