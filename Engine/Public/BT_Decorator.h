#pragma once
#include "BT_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Decorator : public CBT_Node
{
	using Super = CBT_Node;
public:
	enum class DecoratorType { IF, WHILE, REPEAT, TYPE_END };

protected:
	CBT_Decorator();
	CBT_Decorator(const CBT_Decorator& rhs);
	virtual ~CBT_Decorator() = default;
	
public:
	HRESULT				Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType);

	virtual void		OnStart()	override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()		override;

	virtual void		Reset()		override							{ m_vecChildren[0]->Reset(); }

	virtual BT_NODETYPE	NodeType()	override							{ return BT_NODETYPE::DECORATOR; }

protected:
	DecoratorType		m_eDecoratorType = DecoratorType::TYPE_END;

public:
	static	CBT_Decorator* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType);
	virtual void Free() override;
};

END