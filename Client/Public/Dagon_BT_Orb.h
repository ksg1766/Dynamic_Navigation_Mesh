#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Orb : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_Orb();
	CDagon_BT_Orb(const CDagon_BT_Orb& rhs) = delete;
	virtual ~CDagon_BT_Orb() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	void				HitOrMiss();
	_bool				m_bHitOrMiss;

	_bool				m_bAttacked = false;
	static _int			m_iOrbNum;
	static _bool		m_isOrbLeft;

public:
	static	CDagon_BT_Orb* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END