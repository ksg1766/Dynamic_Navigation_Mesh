#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDemonCaster_BT_StepBack : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDemonCaster_BT_StepBack();
	CDemonCaster_BT_StepBack(const CDemonCaster_BT_StepBack& rhs) = delete;
	virtual ~CDemonCaster_BT_StepBack() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	
	_bool				IsInRange();
	_bool				IsZeroHP();

public:
	static	CDemonCaster_BT_StepBack* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;

};

END