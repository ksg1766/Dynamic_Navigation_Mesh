#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Wave : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_Wave();
	CDagon_BT_Wave(const CDagon_BT_Wave& rhs) = delete;
	virtual ~CDagon_BT_Wave() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();

	_bool				m_bAttacked = false;

public:
	static	CDagon_BT_Wave* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END