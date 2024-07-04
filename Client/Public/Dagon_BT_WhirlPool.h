#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_WhirlPool : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_WhirlPool();
	CDagon_BT_WhirlPool(const CDagon_BT_WhirlPool& rhs) = delete;
	virtual ~CDagon_BT_WhirlPool() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	void				HitOrMiss();
	_bool				m_bHitOrMiss;

public:
	static	CDagon_BT_WhirlPool* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END