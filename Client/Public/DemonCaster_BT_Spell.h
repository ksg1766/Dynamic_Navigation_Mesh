#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDemonCaster_BT_Spell final : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDemonCaster_BT_Spell();
	CDemonCaster_BT_Spell(const CDemonCaster_BT_Spell& rhs) = delete;
	virtual ~CDemonCaster_BT_Spell() = default;
	
	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;
	_bool				IsInRange();
	_bool				IsInSight();
	_bool				IsZeroHP();

private:
	class CWaterShield*	m_pWaterShield = nullptr;

public:
	static	CDemonCaster_BT_Spell* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END