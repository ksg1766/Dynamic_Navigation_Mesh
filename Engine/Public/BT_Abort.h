#pragma once
#include "BT_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Abort : public CBT_Node
{
	using Super = CBT_Node;
public:
	enum class AbortType { SELF, LOWERPRIORITY, BOTH, TYPE_END };

protected:
	CBT_Abort();
	CBT_Abort(const CBT_Abort& rhs);
	virtual ~CBT_Abort() = default;
	
public:
	HRESULT				Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, AbortType	eAbortType);

	virtual void		OnStart()								override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)		override;
	virtual void		OnEnd()									override;

	virtual BT_NODETYPE	NodeType() override { return BT_NODETYPE::ABORT; }

	void				SetFunctionPtr(function<_bool()> ftAbortCondition) { m_ftAbortCondition = ftAbortCondition; }

protected:
	AbortType	m_eAbortType = AbortType::TYPE_END;
	
	function<_bool()>	m_ftAbortCondition;

public:
	virtual void Free() override;
};

END