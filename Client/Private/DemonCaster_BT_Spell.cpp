#include "stdafx.h"
#include "DemonCaster_BT_Spell.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "WaterShield.h"

CDemonCaster_BT_Spell::CDemonCaster_BT_Spell()
{
}

void CDemonCaster_BT_Spell::OnStart()
{
	Super::OnStart(0);

	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_demoncaster_atk_cast_blizzard.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.3f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CDemonCaster_BT_Spell::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
	{
		
		return BT_FAIL;
	}

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(vTargetPos);

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		const Vec3& vPosition = m_pGameObject->GetTransform()->GetPosition();
		const Vec3& vRight = m_pGameObject->GetTransform()->GetRight();
		_int iNaveMeshIndex = m_pGameObject->GetNavMeshAgent()->GetCurrentIndex();
		
		CGameObject* pMonster = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_HellHound"), LAYERTAG::UNIT_GROUND);
		pMonster->GetNavMeshAgent()->SetCurrentIndex(iNaveMeshIndex);
		pMonster->GetTransform()->SetPosition(vPosition + 0.5f * vRight);
		
		pMonster = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Goblin"), LAYERTAG::UNIT_GROUND);
		pMonster->GetNavMeshAgent()->SetCurrentIndex(iNaveMeshIndex);
		pMonster->GetTransform()->SetPosition(vPosition - 0.5f * vRight);

		if (!m_pWaterShield)
		{
			if (!m_pWaterShield->IsDead())
			{
				m_pGameInstance->DeleteObject(m_pWaterShield);
				m_pWaterShield = nullptr;
			}
		}

		return BT_SUCCESS;
	}

	if (!m_pWaterShield && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.1f)
	{
		m_pWaterShield = static_cast<CWaterShield*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_WaterShield"), LAYERTAG::IGNORECOLLISION));
		m_pWaterShield->SetScaleRatio(0.3f);
		m_pWaterShield->GetTransform()->Translate(m_pGameObject->GetTransform()->GetPosition());
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CDemonCaster_BT_Spell::OnEnd()
{
	Super::OnEnd();
}

void CDemonCaster_BT_Spell::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CDemonCaster_BT_Spell::IsInSight()
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

_bool CDemonCaster_BT_Spell::IsZeroHP()
{
	if(static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CDemonCaster_BT_Spell* CDemonCaster_BT_Spell::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDemonCaster_BT_Spell* pInstance = new CDemonCaster_BT_Spell;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDemonCaster_BT_Spell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster_BT_Spell::Free()
{
	Super::Free();
}