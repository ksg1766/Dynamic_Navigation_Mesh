#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CHellHound_BT_Attack final : public CBT_Action
{
	using Super = CBT_Action;
private:
	CHellHound_BT_Attack();
	CHellHound_BT_Attack(const CHellHound_BT_Attack& rhs) = delete;
	virtual ~CHellHound_BT_Attack() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;

	_bool				IsZeroHP();

public:
	static	CHellHound_BT_Attack* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END