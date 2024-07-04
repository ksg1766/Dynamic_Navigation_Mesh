#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CMoloch_BT_Geyser1 : public CBT_Action
{
	using Super = CBT_Action;
private:
	CMoloch_BT_Geyser1();
	CMoloch_BT_Geyser1(const CMoloch_BT_Geyser1& rhs) = delete;
	virtual ~CMoloch_BT_Geyser1() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	void				FirstRun();
	void				SecondRun();

	virtual void		ConditionalAbort(const _float& fTimeDelta);
	_bool				IsZeroHP();

	CGameObject*		GetTarget();

	Vec3				m_vTargetPos;
	Vec3				m_vTargetForward;
	Vec3				m_vTargetRight;

	_bool				m_bAttack;
	_bool				m_bCrystalUp;
	vector<CGameObject*>m_vecCrystal;

public:
	static	CMoloch_BT_Geyser1* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END