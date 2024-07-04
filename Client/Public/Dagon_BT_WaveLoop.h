#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_WaveLoop : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_WaveLoop();
	CDagon_BT_WaveLoop(const CDagon_BT_WaveLoop& rhs) = delete;
	virtual ~CDagon_BT_WaveLoop() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	void				HitOrMiss();
	_bool				m_bHitOrMiss;
	CGameObject*		m_pWaterShield = nullptr;

public:
	static	CDagon_BT_WaveLoop* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END