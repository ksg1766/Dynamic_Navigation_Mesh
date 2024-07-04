#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CHellHound_BT_Spawn : public CBT_Action
{
	using Super = CBT_Action;
protected:
	CHellHound_BT_Spawn();
	CHellHound_BT_Spawn(const CHellHound_BT_Spawn& rhs) = delete;
	virtual ~CHellHound_BT_Spawn() = default;

	virtual void		ConditionalAbort(const _float& fTimeDelta);

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

public:
	static	CHellHound_BT_Spawn* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END