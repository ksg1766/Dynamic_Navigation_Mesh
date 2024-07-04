#include "stdafx.h"
#include "DemonCaster_BT_Spawn.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

CDemonCaster_BT_Spawn::CDemonCaster_BT_Spawn()
{
}

void CDemonCaster_BT_Spawn::ConditionalAbort(const Engine::_float& fTimeDelta)
{
}


void CDemonCaster_BT_Spawn::OnStart()
{
	Super::OnStart();
}

CBT_Node::BT_RETURN CDemonCaster_BT_Spawn::OnUpdate(const _float& fTimeDelta)
{
	return BT_RETURN();
}

void CDemonCaster_BT_Spawn::OnEnd()
{
	Super::OnEnd();
}

CDemonCaster_BT_Spawn* CDemonCaster_BT_Spawn::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDemonCaster_BT_Spawn* pInstance = new CDemonCaster_BT_Spawn;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDemonCaster_BT_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster_BT_Spawn::Free()
{
	Super::Free();
}