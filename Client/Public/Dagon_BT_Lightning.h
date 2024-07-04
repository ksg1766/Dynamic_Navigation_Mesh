#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Lightning : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_Lightning();
	CDagon_BT_Lightning(const CDagon_BT_Lightning& rhs) = delete;
	virtual ~CDagon_BT_Lightning() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	void				HitOrMiss();
	_bool				m_bHitOrMiss;

	_bool				m_bAttacked = false;
	_bool				m_bSoundIndex = 0;

public:
	static	CDagon_BT_Lightning* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END