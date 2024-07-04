#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDemonCaster_BT_Dead : public CBT_Action
{
	using Super = CBT_Action;
protected:
	CDemonCaster_BT_Dead();
	CDemonCaster_BT_Dead(const CDemonCaster_BT_Dead& rhs) = delete;
	virtual ~CDemonCaster_BT_Dead() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;

private:
	_bool				m_bDissolveFlag = false;

public:
	static	CDemonCaster_BT_Dead* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END