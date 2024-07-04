#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_TremorPulse : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_TremorPulse();
	CMoloch_BT_TremorPulse(const CMoloch_BT_TremorPulse& rhs) = delete;
	virtual ~CMoloch_BT_TremorPulse() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

public:
	static	CMoloch_BT_TremorPulse* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END