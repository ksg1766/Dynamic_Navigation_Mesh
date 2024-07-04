#include "stdafx.h"
#include "Goblin_BT_Spawn.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CGoblin_BT_Spawn::CGoblin_BT_Spawn()
{
}

void CGoblin_BT_Spawn::ConditionalAbort(const Engine::_float& fTimeDelta)
{
}


void CGoblin_BT_Spawn::OnStart()
{
	Super::OnStart();
}

CBT_Node::BT_RETURN CGoblin_BT_Spawn::OnUpdate(const _float& fTimeDelta)
{
	return BT_RETURN();
}

void CGoblin_BT_Spawn::OnEnd()
{
	Super::OnEnd();
}

CGoblin_BT_Spawn* CGoblin_BT_Spawn::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Spawn* pInstance = new CGoblin_BT_Spawn;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Spawn::Free()
{
	Super::Free();
}