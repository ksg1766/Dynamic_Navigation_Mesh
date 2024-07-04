#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_Chase : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_Chase();
	CMoloch_BT_Chase(const CMoloch_BT_Chase& rhs) = delete;
	virtual ~CMoloch_BT_Chase() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);
	_bool				IsInRange();

private:
	_float			m_fDefault = 0.8f;
	_float			m_fDefault_Timer = 0.f;

public:
	static	CMoloch_BT_Chase* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END