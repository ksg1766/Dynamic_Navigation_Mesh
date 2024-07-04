#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CGoblin_BT_Idle : public CBT_Action
{
	using Super = CBT_Action;
private:
	CGoblin_BT_Idle();
	CGoblin_BT_Idle(const CGoblin_BT_Idle& rhs) = delete;
	virtual ~CGoblin_BT_Idle() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	
	_bool				IsAggro();
	_bool				IsZeroHP();

public:
	static	CGoblin_BT_Idle* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END