#pragma once
#include "Client_Defines.h"
#include "BT_Action.h"

BEGIN(Client)

class CDagon_BT_Dead : public CBT_Action
{
	using Super = CBT_Action;
protected:
	CDagon_BT_Dead();
	CDagon_BT_Dead(const CDagon_BT_Dead& rhs) = delete;
	virtual ~CDagon_BT_Dead() = default;

	virtual void		OnStart()							override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()								override;

private:
	virtual void		ConditionalAbort(const _float& fTimeDelta)	override;

private:
	_bool				m_bDissolveFlag = false;

public:
	static	CDagon_BT_Dead* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END