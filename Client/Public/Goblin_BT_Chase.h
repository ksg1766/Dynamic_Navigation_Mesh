#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CGoblin_BT_Chase final : public CBT_Action
{
	using Super = CBT_Action;
private:
	CGoblin_BT_Chase();
	CGoblin_BT_Chase(const CGoblin_BT_Chase& rhs) = delete;
	virtual ~CGoblin_BT_Chase() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;
	_bool				IsInSuicideRange();
	_bool				IsInSight();
	_bool				IsZeroHP();

public:
	static	CGoblin_BT_Chase* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END