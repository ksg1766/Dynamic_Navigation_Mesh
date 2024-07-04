#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_Swing2 : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_Swing2();
	CMoloch_BT_Swing2(const CMoloch_BT_Swing2& rhs) = delete;
	virtual ~CMoloch_BT_Swing2() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	CGameObject*		GetTarget();


public:
	static	CMoloch_BT_Swing2* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END