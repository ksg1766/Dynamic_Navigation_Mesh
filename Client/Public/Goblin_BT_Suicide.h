#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CGoblin_BT_Suicide final : public CBT_Action
{
	using Super = CBT_Action;
private:
	CGoblin_BT_Suicide();
	CGoblin_BT_Suicide(const CGoblin_BT_Suicide& rhs) = delete;
	virtual ~CGoblin_BT_Suicide() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;

	_bool				IsZeroHP();

public:
	static	CGoblin_BT_Suicide* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END