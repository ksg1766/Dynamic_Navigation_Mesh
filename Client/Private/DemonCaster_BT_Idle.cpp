#include "stdafx.h"
#include "DemonCaster_BT_Idle.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CDemonCaster_BT_Idle::CDemonCaster_BT_Idle()
{
}

void CDemonCaster_BT_Idle::OnStart()
{
	Super::OnStart(0);
	StartIdleCoolDown();
}

CBT_Node::BT_RETURN CDemonCaster_BT_Idle::OnUpdate(const _float& fTimeDelta)
{
	// TODO: �̰� ������ �ܰ迡�� �Ǵ��� �� �ֵ��� �ؾ� ��. // Abort ��ȯ ����� �޾Ƽ� ������Ű����.
	if (IsZeroHP())
		return BT_FAIL;

	if (IsAggro())
		return BT_FAIL;

	ConditionalAbort(fTimeDelta);
	// �°ų� �þ߾ȿ� �÷��̾� ������ FAIL;
	// �þ� üũ�� sequence�� ����ؼ� ������ ���� �ʴ� �� ���� Ŭ������ �Լ��� ����� �� ��.
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (tIdleCoolDown == hashBlackBoard.end())
		return BT_SUCCESS;

	RunIdleCoolDown(fTimeDelta);

	return BT_RUNNING;
}

void CDemonCaster_BT_Idle::OnEnd()
{
	AbortIdleCoolDown();
	Super::OnEnd();
}

void CDemonCaster_BT_Idle::ConditionalAbort(const _float& fTimeDelta)
{
}

void CDemonCaster_BT_Idle::StartIdleCoolDown()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (tIdleCoolDown == hashBlackBoard.end())
	{
		tagBlackBoardData<_float>* tIdleCool = new tagBlackBoardData<_float>(3.f);
		hashBlackBoard.emplace(TEXT("IdleCoolDown"), tIdleCool);
	}
	else
	{
		__debugbreak();
		SET_VALUE(_float, tIdleCoolDown, 3.f);
	}
}

void CDemonCaster_BT_Idle::AbortIdleCoolDown()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (tIdleCoolDown != hashBlackBoard.end())
	{
		hashBlackBoard.erase(tIdleCoolDown);
	}
}

void CDemonCaster_BT_Idle::RunIdleCoolDown(const _float& fTimeDelta)
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (0.f > *GET_VALUE(_float, tIdleCoolDown))
	{
		hashBlackBoard.erase(tIdleCoolDown);
	}
	else
	{
		SET_VALUE(_float, tIdleCoolDown, *GET_VALUE(_float, tIdleCoolDown) - fTimeDelta);
	}
}

_bool CDemonCaster_BT_Idle::IsAggro()
{
	map<LAYERTAG, class CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	const map<LAYERTAG, class CLayer*>::iterator& pPlayerLayer = mapLayers.find(LAYERTAG::PLAYER);

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tSight = hashBlackBoard.find(TEXT("Sight"));
	const auto& tTarget = hashBlackBoard.find(TEXT("Target"));

	CGameObject* pPlayer = pPlayerLayer->second->GetGameObjects().front();
	if ((pPlayer->GetTransform()->GetPosition() - m_pGameObject->GetTransform()->GetPosition()).Length() < *GET_VALUE(_float, tSight))	// �þ߿� �ִٸ�
	{
		if (tTarget == hashBlackBoard.end())	// Ÿ���� Ű���� �����忡 ���ٸ�(������ �������� �����͵� ����� ��) Ű�� �߰�����.
		{
			tagBlackBoardData<CGameObject*>* pTarget = new tagBlackBoardData<CGameObject*>(pPlayer);

			hashBlackBoard.emplace(TEXT("Target"), pTarget);
		}

		return true;
	}
	else // �þ߿� ���ٸ�
	{
		BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();

		if (tTarget != hashBlackBoard.end())	// �ٵ� Ű���� �ִٸ� ����
		{
			hashBlackBoard.erase(tTarget);
		}

		return false;
	}
}

_bool CDemonCaster_BT_Idle::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CDemonCaster_BT_Idle* CDemonCaster_BT_Idle::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDemonCaster_BT_Idle* pInstance = new CDemonCaster_BT_Idle;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDemonCaster_BT_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster_BT_Idle::Free()
{
	Super::Free();
}


