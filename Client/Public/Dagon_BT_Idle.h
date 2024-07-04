#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Idle : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_Idle();
	CDagon_BT_Idle(const CDagon_BT_Idle& rhs) = delete;
	virtual ~CDagon_BT_Idle() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	void				StartIdleCoolDown();
	void				AbortIdleCoolDown();
	void				RunIdleCoolDown(const _float& fTimeDelta);

	_bool				IsZeroHP();

public:
	static	CDagon_BT_Idle* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END