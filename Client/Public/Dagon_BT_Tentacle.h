#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Tentacle : public CBT_Action
{
	using Super = CBT_Action;
private:
	CDagon_BT_Tentacle();
	CDagon_BT_Tentacle(const CDagon_BT_Tentacle& rhs) = delete;
	virtual ~CDagon_BT_Tentacle() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta);

	_bool				IsZeroHP();
	_bool				m_bAttacked = false;

public:
	static	CDagon_BT_Tentacle* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END