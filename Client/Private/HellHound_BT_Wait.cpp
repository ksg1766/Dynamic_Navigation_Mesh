#include "stdafx.h"
#include "HellHound_BT_Wait.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CHellHound_BT_Wait::CHellHound_BT_Wait()
{
}

void CHellHound_BT_Wait::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CHellHound_BT_Wait::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (!IsInRange())
	{
		return BT_FAIL;
	}

	if (!m_bSoundOn && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.1f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellhound_taunt_vo_01.ogg"), CHANNELID::CHANNEL_ENEMY5, 0.3f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.8f)
	{
		return BT_SUCCESS;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CHellHound_BT_Wait::OnEnd()
{
	Super::OnEnd();
}

void CHellHound_BT_Wait::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CHellHound_BT_Wait::IsInRange()
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

_bool CHellHound_BT_Wait::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CHellHound_BT_Wait* CHellHound_BT_Wait::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellHound_BT_Wait* pInstance = new CHellHound_BT_Wait;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellHound_BT_Wait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellHound_BT_Wait::Free()
{
	Super::Free();
}