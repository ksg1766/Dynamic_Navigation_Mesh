#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CHellBrute_BT_Idle : public CBT_Action
{
	using Super = CBT_Action;
private:
	CHellBrute_BT_Idle();
	CHellBrute_BT_Idle(const CHellBrute_BT_Idle& rhs) = delete;
	virtual ~CHellBrute_BT_Idle() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	void				StartIdleCoolDown();
	void				AbortIdleCoolDown();
	void				RunIdleCoolDown(const _float& fTimeDelta);
	
	_bool				IsAggro();
	_bool				IsZeroHP();

public:
	static	CHellBrute_BT_Idle* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END