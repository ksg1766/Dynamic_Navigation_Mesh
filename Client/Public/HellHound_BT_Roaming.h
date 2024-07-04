#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CHellHound_BT_Roaming : public CBT_Action
{
	using Super = CBT_Action;
private:
	CHellHound_BT_Roaming();
	CHellHound_BT_Roaming(const CHellHound_BT_Roaming& rhs) = delete;
	virtual ~CHellHound_BT_Roaming() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	void				SetRoamingPoint();
	void				AbortRoaming();

	_bool				IsAggro();
	_bool				IsZeroHP();

public:
	static	CHellHound_BT_Roaming* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END