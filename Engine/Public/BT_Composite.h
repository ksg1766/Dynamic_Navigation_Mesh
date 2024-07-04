#pragma once
#include "BT_Node.h"

BEGIN(Engine)

//class CBT_Abort;
class ENGINE_DLL CBT_Composite : public CBT_Node
{
	using Super = CBT_Node;
public:
	enum class CompositeType { SELECTOR, SEQUENCE, TYPE_END };

protected:
	CBT_Composite();
	CBT_Composite(const CBT_Composite& rhs);
	virtual ~CBT_Composite() = default;
	
public:
	HRESULT				Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, CompositeType eCompositeType);

	virtual void		OnStart()	override;
	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta)	override;
	virtual void		OnEnd()		override;

	virtual BT_NODETYPE	NodeType()	override			{ return BT_NODETYPE::COMPOSITE; }
	//void				AddAbort(CBT_Abort* pAbort)	{ m_vecConditionalAbort.push_back(pAbort); }

	virtual void		Reset()		override;

protected:
	CompositeType		m_eCompositeType = CompositeType::TYPE_END;
	vector<CBT_Node*>::iterator	m_RunningChild;

	//vector<CBT_Abort*>	m_vecConditionalAbort;

public:
	static	CBT_Composite* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, CompositeType eCompositeType);
	virtual void Free() override;
};

END