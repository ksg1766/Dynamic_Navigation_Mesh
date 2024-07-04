#include "stdafx.h"
#include "HellBrute_BT_Spawn.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CHellBrute_BT_Spawn::CHellBrute_BT_Spawn()
{
}

void CHellBrute_BT_Spawn::ConditionalAbort(const Engine::_float& fTimeDelta)
{
}


void CHellBrute_BT_Spawn::OnStart()
{
	Super::OnStart();
}

CBT_Node::BT_RETURN CHellBrute_BT_Spawn::OnUpdate(const _float& fTimeDelta)
{
	return BT_RETURN();
}

void CHellBrute_BT_Spawn::OnEnd()
{
	Super::OnEnd();
}

CHellBrute_BT_Spawn* CHellBrute_BT_Spawn::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellBrute_BT_Spawn* pInstance = new CHellBrute_BT_Spawn;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellBrute_BT_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute_BT_Spawn::Free()
{
	Super::Free();
}