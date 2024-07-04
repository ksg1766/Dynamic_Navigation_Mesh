#include "stdafx.h"
#include "Moloch_BT_Geyser2.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "TremorCrystal.h"
#include "Particle.h"
#include "ParticleController.h"

CMoloch_BT_Geyser2::CMoloch_BT_Geyser2()
{
}

void CMoloch_BT_Geyser2::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CMoloch_BT_Geyser2::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.97f)
	{
		return BT_SUCCESS;
	}

	if (!m_bSoundOn && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.13f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_full_geyser_jump.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Geyser2::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Geyser2::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Geyser2::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Geyser2::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Geyser2* CMoloch_BT_Geyser2::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Geyser2* pInstance = new CMoloch_BT_Geyser2;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Geyser2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Geyser2::Free()
{
	Super::Free();
}