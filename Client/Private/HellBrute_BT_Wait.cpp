#include "stdafx.h"
#include "HellBrute_BT_Wait.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CHellBrute_BT_Wait::CHellBrute_BT_Wait()
{
}

void CHellBrute_BT_Wait::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CHellBrute_BT_Wait::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (!IsInRange())
	{
		return BT_FAIL;
	}

	if (!m_bSoundOn && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellbrute_powershot_charge.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.3f)))
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

void CHellBrute_BT_Wait::OnEnd()
{
	Super::OnEnd();
}

void CHellBrute_BT_Wait::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CHellBrute_BT_Wait::IsInRange()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& meleeRange = hashBlackBoard.find(TEXT("MeleeRange"));
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
	const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();
	Vec3 vDist = vTargetPos - vCurPos;

	if (vDist.Length() < *GET_VALUE(_float, meleeRange))
		return true;
	else
		return false;
}

_bool CHellBrute_BT_Wait::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CHellBrute_BT_Wait* CHellBrute_BT_Wait::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellBrute_BT_Wait* pInstance = new CHellBrute_BT_Wait;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellBrute_BT_Wait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute_BT_Wait::Free()
{
	Super::Free();
}