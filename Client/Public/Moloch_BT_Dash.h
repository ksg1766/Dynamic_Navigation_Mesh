#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_Dash : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_Dash();
	CMoloch_BT_Dash(const CMoloch_BT_Dash& rhs) = delete;
	virtual ~CMoloch_BT_Dash() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	CGameObject*		GetOrAddTarget();

	Vec3				m_vTargetPos;

	_int				m_iFrameCounter = 0;
	_bool				m_bAttack;

public:
	static	CMoloch_BT_Dash* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END