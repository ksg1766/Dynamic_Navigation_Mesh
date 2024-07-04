#include "BT_Decorator.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CBT_Decorator::CBT_Decorator()
{
}

CBT_Decorator::CBT_Decorator(const CBT_Decorator& rhs)
	:Super(rhs)
{
}

HRESULT CBT_Decorator::Initialize(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
{
	Super::Initialize(pGameObject, pBehaviorTree, pController);
	m_eDecoratorType = eDecoratorType;

	return S_OK;
}

void CBT_Decorator::OnStart()
{
	Super::OnStart();
	/*if (DecoratorType::REPEAT == m_eDecoratorType)
		m_iCounter = m_iLimitCount;*/
}

CBT_Node::BT_RETURN CBT_Decorator::OnUpdate(const _float& fTimeDelta)
{
	/*switch (m_eDecoratorType)
	{
	case DecoratorType::IF:
	{
		if(true == m_ftCondition())
			return m_vecChildren.front()->Tick(fTimeDelta);
		else
			return BT_FAIL;
	}
	break;
	case DecoratorType::WHILE:
	{
		if (true == m_ftCondition())
		{
			m_vecChildren.front()->Tick(fTimeDelta);
			return BT_RUNNING;
		}
		else
			return BT_FAIL;
	}
	break;
	case DecoratorType::REPEAT:
	{
		if (0 < m_iCounter)
		{
			m_vecChildren.front()->Tick(fTimeDelta);
			return BT_RUNNING;
		}
		else
			return BT_SUCCESS;
	}
	break;
	}*/

	//
	return BT_SUCCESS;
}

void CBT_Decorator::OnEnd()
{
	Super::OnEnd();
}

CBT_Decorator* CBT_Decorator::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
{
	CBT_Decorator* pInstance = new CBT_Decorator;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
	{
		MSG_BOX("Failed to Created : CBT_Decorator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_Decorator::Free()
{
	Super::Free();
}