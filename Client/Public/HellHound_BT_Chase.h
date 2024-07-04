#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CHellHound_BT_Chase : public CBT_Action
{
	using Super = CBT_Action;
private:
	CHellHound_BT_Chase();
	CHellHound_BT_Chase(const CHellHound_BT_Chase& rhs) = delete;
	virtual ~CHellHound_BT_Chase() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	_bool				IsInRange();
	_bool				IsInSight();
	_bool				IsZeroHP();

public:
	static	CHellHound_BT_Chase* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END