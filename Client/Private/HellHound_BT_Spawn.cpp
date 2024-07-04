#include "stdafx.h"
#include "HellHound_BT_Spawn.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CHellHound_BT_Spawn::CHellHound_BT_Spawn()
{
}

void CHellHound_BT_Spawn::ConditionalAbort(const Engine::_float& fTimeDelta)
{
}


void CHellHound_BT_Spawn::OnStart()
{
	Super::OnStart();
}

CBT_Node::BT_RETURN CHellHound_BT_Spawn::OnUpdate(const _float& fTimeDelta)
{
	return BT_RETURN();
}

void CHellHound_BT_Spawn::OnEnd()
{
	Super::OnEnd();
}

CHellHound_BT_Spawn* CHellHound_BT_Spawn::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellHound_BT_Spawn* pInstance = new CHellHound_BT_Spawn;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellHound_BT_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellHound_BT_Spawn::Free()
{
	Super::Free();
}