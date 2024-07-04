#include "stdafx.h"
#include "Moloch_BT_Swipe.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CMoloch_BT_Swipe::CMoloch_BT_Swipe()
{
}

void CMoloch_BT_Swipe::OnStart()
{
	Super::OnStart(0);

	const Vec3& vTargetPos = GetTarget()->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(vTargetPos);
}

CBT_Node::BT_RETURN CMoloch_BT_Swipe::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.95f)
	{
		return BT_SUCCESS;
	}

	if (!m_bSoundOn && m_fTimeSum > 0.2f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_swipe_01.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Swipe::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Swipe::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Swipe::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Swipe::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Swipe* CMoloch_BT_Swipe::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Swipe* pInstance = new CMoloch_BT_Swipe;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Swipe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Swipe::Free()
{
	Super::Free();
}