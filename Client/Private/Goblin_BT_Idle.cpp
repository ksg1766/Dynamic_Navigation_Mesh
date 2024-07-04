#include "stdafx.h"
#include "Goblin_BT_Idle.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CGoblin_BT_Idle::CGoblin_BT_Idle()
{
}

void CGoblin_BT_Idle::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CGoblin_BT_Idle::OnUpdate(const _float& fTimeDelta)
{
	// TODO: 이걸 시퀀스 단계에서 판단할 수 있도록 해야 함. // Abort 반환 결과를 받아서 정지시키도록.
	if (IsZeroHP())
		return BT_FAIL;

	if (IsAggro())
		return BT_FAIL;

	return BT_RUNNING;
}

void CGoblin_BT_Idle::OnEnd()
{
	Super::OnEnd();
}

void CGoblin_BT_Idle::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CGoblin_BT_Idle::IsAggro()
{
	map<LAYERTAG, class CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	const map<LAYERTAG, class CLayer*>::iterator& pPlayerLayer = mapLayers.find(LAYERTAG::PLAYER);

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tSight = hashBlackBoard.find(TEXT("Sight"));
	const auto& tTarget = hashBlackBoard.find(TEXT("Target"));

	CGameObject* pPlayer = pPlayerLayer->second->GetGameObjects().front();
	if ((pPlayer->GetTransform()->GetPosition() - m_pGameObject->GetTransform()->GetPosition()).Length() < *GET_VALUE(_float, tSight))	// 시야에 있다면
	{
		if (tTarget == hashBlackBoard.end())	// 타겟의 키값이 블랙보드에 없다면(이전에 없었으면 데이터도 없어야 함) 키값 추가해줌.
		{
			tagBlackBoardData<CGameObject*>* pTarget = new tagBlackBoardData<CGameObject*>(pPlayer);

			hashBlackBoard.emplace(TEXT("Target"), pTarget);
		}

		return true;
	}
	else // 시야에 없다면
	{
		BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();

		if (tTarget != hashBlackBoard.end())	// 근데 키값이 있다면 제거
		{
			hashBlackBoard.erase(tTarget);
		}

		return false;
	}
}

_bool CGoblin_BT_Idle::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGoblin_BT_Idle* CGoblin_BT_Idle::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Idle* pInstance = new CGoblin_BT_Idle;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Idle::Free()
{
	Super::Free();
}


