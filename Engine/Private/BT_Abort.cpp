#include "BT_Abort.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CBT_Abort::CBT_Abort()
{
}

CBT_Abort::CBT_Abort(const CBT_Abort& rhs)
	:Super(rhs)
{
}

HRESULT CBT_Abort::Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, AbortType eAbortType)
{
	Super::Initialize(pGameObject, pBehaviorTree, pController);
	m_eAbortType = eAbortType;

	return S_OK;
}

void CBT_Abort::OnStart()
{
}

CBT_Node::BT_RETURN CBT_Abort::OnUpdate(const _float& fTimeDelta)
{
	/*switch (m_eAbortType)
	{
	case AbortType::SELF:
	{
		if (true == m_ftAbortCondition())
			
		else
			
	}
	break;
	case AbortType::LOWERPRIORITY:
	{
		if (true == m_ftAbortCondition())

		else
			
	}
	break;
	case AbortType::BOTH:
	{
		if (true == m_ftAbortCondition())

		else
			
	}
	break;
	}*/

	return BT_SUCCESS;
}

void CBT_Abort::OnEnd()
{
}

void CBT_Abort::Free()
{
	Super::Free();
}