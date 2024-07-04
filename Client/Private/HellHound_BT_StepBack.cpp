#include "stdafx.h"
#include "HellHound_BT_StepBack.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CHellHound_BT_StepBack::CHellHound_BT_StepBack()
{
}

void CHellHound_BT_StepBack::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CHellHound_BT_StepBack::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		return BT_SUCCESS;
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f && m_fTimeSum < m_vecAnimIndexTime[0].second * 0.5f)
	{
		CMonsterController* pController = static_cast<CMonsterController*>(m_pController);

		BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
		const auto& target = hashBlackBoard.find(TEXT("Target"));

		const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
		const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();

		pController->Look(vTargetPos, 10 * fTimeDelta);
		pController->GetTranslateMessage(-(vTargetPos - vCurPos));

		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellhound_evade_01.ogg"), CHANNELID::CHANNEL_ENEMY4, 0.3f)))
			__debugbreak();
	}
	// 
	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CHellHound_BT_StepBack::OnEnd()
{
	Super::OnEnd();
}

void CHellHound_BT_StepBack::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CHellHound_BT_StepBack::IsInRange()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& attackRange = hashBlackBoard.find(TEXT("AttackRange"));
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
	const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();
	Vec3 vDist = vTargetPos - vCurPos;

	if (vDist.Length() < *GET_VALUE(_float, attackRange))
		return true;
	else
		return false;
}

_bool CHellHound_BT_StepBack::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CHellHound_BT_StepBack* CHellHound_BT_StepBack::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellHound_BT_StepBack* pInstance = new CHellHound_BT_StepBack;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellHound_BT_StepBack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellHound_BT_StepBack::Free()
{
	Super::Free();
}