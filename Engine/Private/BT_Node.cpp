#include "BT_Composite.h"
#include "BT_Abort.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CBT_Node::CBT_Node()
{
	m_pGameInstance = GET_INSTANCE(CGameInstance);
}

CBT_Node::CBT_Node(const CBT_Node& rhs)
	:Super(rhs)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CBT_Node::Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController)
{
	m_pGameObject = pGameObject;
	m_pController = pController;
	m_pBehaviorTree = pBehaviorTree;

	return S_OK;
}

CBT_Node::BT_RETURN CBT_Node::Tick(const _float& fTimeDelta)
{
	if (BT_RETURN::BT_RUNNING != m_eReturn)
		OnStart();

	m_eReturn = OnUpdate(fTimeDelta);

	if (BT_RETURN::BT_RUNNING != m_eReturn)
		OnEnd();

	return m_eReturn;
}

HRESULT CBT_Node::AddChild(CBT_Node* pChild)
{
	if (BT_NODETYPE::ACTION == NodeType())
		return E_FAIL;
	else if (BT_NODETYPE::DECORATOR == NodeType() && !m_vecChildren.empty())
		return E_FAIL; 
	else if (BT_NODETYPE::COMPOSITE == NodeType())
	{
		m_vecChildren.push_back(pChild);
		/*if (BT_NODETYPE::ABORT == pChild->NodeType())
			static_cast<CBT_Composite*>(this)->SetAbort(static_cast<CBT_Abort*>(pChild));*/
	}
	else
		m_vecChildren.push_back(pChild);

	return S_OK;
}

void CBT_Node::Free()
{
	for(auto& iter : m_vecChildren)
		Safe_Release(iter);

	Super::Free();
	GET_INSTANCE(CGameInstance);
}