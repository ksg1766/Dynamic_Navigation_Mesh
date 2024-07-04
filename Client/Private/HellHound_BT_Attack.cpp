#include "stdafx.h"
#include "HellHound_BT_Attack.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CHellHound_BT_Attack::CHellHound_BT_Attack()
{
}

void CHellHound_BT_Attack::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CHellHound_BT_Attack::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
		pController->GetAttackMessage();
		
		return BT_SUCCESS;
	}
	else if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.3f && m_fTimeSum < m_vecAnimIndexTime[0].second * 0.6f)
	{
		CMonsterController* pController = static_cast<CMonsterController*>(m_pController);

		BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
		const auto& target = hashBlackBoard.find(TEXT("Target"));

		const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
		const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();

		pController->GetMoveMessage(vTargetPos - vCurPos);

		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellhound_atk_bite_01.ogg"), CHANNELID::CHANNEL_ENEMY3, 0.3f)))
			__debugbreak();
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
	// BT_FAIL 은 중간에 맞았을 때 해주자
}

void CHellHound_BT_Attack::OnEnd()
{
	Super::OnEnd();
}

void CHellHound_BT_Attack::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CHellHound_BT_Attack::IsZeroHP()
{
	if(static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CHellHound_BT_Attack* CHellHound_BT_Attack::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellHound_BT_Attack* pInstance = new CHellHound_BT_Attack;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellHound_BT_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellHound_BT_Attack::Free()
{
	Super::Free();
}