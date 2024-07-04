#include "stdafx.h"
#include "Goblin_BT_Chase.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CGoblin_BT_Chase::CGoblin_BT_Chase()
{
}

void CGoblin_BT_Chase::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CGoblin_BT_Chase::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (IsInSuicideRange())
		return BT_SUCCESS;

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
	const Vec3& vCurrentPos = m_pGameObject->GetTransform()->GetPosition();

	Vec3 vChaseDir = vTargetPos - vCurrentPos;
	vChaseDir.Normalize();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->GetMaxSpeedMessage();
	pController->GetMoveMessage(vChaseDir);

	return BT_RUNNING;
}

void CGoblin_BT_Chase::OnEnd()
{
	Super::OnEnd();
}

void CGoblin_BT_Chase::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CGoblin_BT_Chase::IsInSuicideRange()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& suicideRange = hashBlackBoard.find(TEXT("SuicideRange"));
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	if ((GET_VALUE(CGameObject, target)->GetTransform()->GetPosition() - m_pGameObject->GetTransform()->GetPosition()).Length() < *GET_VALUE(_float, suicideRange))
		return true;
	else
		return false;
}

_bool CGoblin_BT_Chase::IsInSight()
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

_bool CGoblin_BT_Chase::IsZeroHP()
{
	if(static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGoblin_BT_Chase* CGoblin_BT_Chase::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Chase* pInstance = new CGoblin_BT_Chase;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Chase::Free()
{
	Super::Free();
}