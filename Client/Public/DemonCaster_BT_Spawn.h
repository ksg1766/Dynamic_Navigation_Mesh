#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDemonCaster_BT_Spawn : public CBT_Action
{
	using Super = CBT_Action;
protected:
	CDemonCaster_BT_Spawn();
	CDemonCaster_BT_Spawn(const CDemonCaster_BT_Spawn& rhs) = delete;
	virtual ~CDemonCaster_BT_Spawn() = default;

	virtual void		ConditionalAbort(const _float& fTimeDelta);

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

public:
	static	CDemonCaster_BT_Spawn* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END