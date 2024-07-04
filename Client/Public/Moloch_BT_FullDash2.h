#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_FullDash2 : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_FullDash2();
	CMoloch_BT_FullDash2(const CMoloch_BT_FullDash2& rhs) = delete;
	virtual ~CMoloch_BT_FullDash2() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	CGameObject*		GetTarget();

	Vec3				m_vTargetPos;

	_int				m_iFrameCounter = 0;
	_int				m_iCrystalCounter = 11;

public:
	static	CMoloch_BT_FullDash2* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END