#include "stdafx.h"
#include "HellBrute_BT_Idle.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CHellBrute_BT_Idle::CHellBrute_BT_Idle()
{
}

void CHellBrute_BT_Idle::OnStart()
{
	Super::OnStart(0);
	StartIdleCoolDown();
	//if (FAILED(m_pGameInstance->StopSound(CHANNELID::CHANNEL_ENEMY2)))
	//	__debugbreak();
}

CBT_Node::BT_RETURN CHellBrute_BT_Idle::OnUpdate(const _float& fTimeDelta)
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

void CHellBrute_BT_Idle::OnEnd()
{
	AbortIdleCoolDown();
	Super::OnEnd();
}

void CHellBrute_BT_Idle::ConditionalAbort(const _float& fTimeDelta)
{
}

void CHellBrute_BT_Idle::StartIdleCoolDown()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (tIdleCoolDown == hashBlackBoard.end())
	{
		tagBlackBoardData<_float>* tStepBackCool = new tagBlackBoardData<_float>(3.f);
		hashBlackBoard.emplace(TEXT("IdleCoolDown"), tStepBackCool);
	}
	else
	{
		__debugbreak();
		SET_VALUE(_float, tIdleCoolDown, 3.f);
	}
}

void CHellBrute_BT_Idle::AbortIdleCoolDown()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tIdleCoolDown = hashBlackBoard.find(TEXT("IdleCoolDown"));

	if (tIdleCoolDown != hashBlackBoard.end())
	{
		hashBlackBoard.erase(tIdleCoolDown);
	}
}

void CHellBrute_BT_Idle::RunIdleCoolDown(const _float& fTimeDelta)
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

_bool CHellBrute_BT_Idle::IsAggro()
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

_bool CHellBrute_BT_Idle::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CHellBrute_BT_Idle* CHellBrute_BT_Idle::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellBrute_BT_Idle* pInstance = new CHellBrute_BT_Idle;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellBrute_BT_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute_BT_Idle::Free()
{
	Super::Free();
}


