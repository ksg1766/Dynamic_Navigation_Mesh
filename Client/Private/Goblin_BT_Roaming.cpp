#include "stdafx.h"
#include "Goblin_BT_Roaming.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CGoblin_BT_Roaming::CGoblin_BT_Roaming()
{
}

void CGoblin_BT_Roaming::OnStart()
{
	Super::OnStart(0);
	SetRoamingPoint();
}

CBT_Node::BT_RETURN CGoblin_BT_Roaming::OnUpdate(const _float& fTimeDelta)
{
	// TODO: �̰� ������ �ܰ迡�� �Ǵ��� �� �ֵ��� �ؾ� ��. // Abort ��ȯ ����� �޾Ƽ� ������Ű����.
	if (IsAggro())
		return BT_FAIL;
	if (IsZeroHP())
		return BT_FAIL;

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tRoamingPoint = hashBlackBoard.find(TEXT("RoamingPoint"));

	const Vec3& vRoamingPoint = *GET_VALUE(Vec3, tRoamingPoint);
	const Vec3& vCurrentPos = m_pGameObject->GetTransform()->GetPosition();

	Vec3 vRoamingDir = vRoamingPoint - vCurrentPos;
	vRoamingDir.Normalize();

	if ((vRoamingPoint - vCurrentPos).Length() < 1.f)
		return BT_SUCCESS;

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->GetMoveMessage(vRoamingDir);

	return BT_RUNNING;
}

void CGoblin_BT_Roaming::OnEnd()
{
	AbortRoaming();
	Super::OnEnd();
}

void CGoblin_BT_Roaming::ConditionalAbort(const _float& fTimeDelta)
{
}

void CGoblin_BT_Roaming::SetRoamingPoint()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tRoamingPoint = hashBlackBoard.find(TEXT("RoamingPoint"));

	if (tRoamingPoint == hashBlackBoard.end())
	{
		srand(time(NULL));
		Vec3 vRandPos(rand() % 4 - 2, 0.f, rand() * 7 % 4 - 2);

		CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
		/*while (!pController->Walkable(vRandPos))
		{
			vRandPos.x = (_float)(((_int)vRandPos.x * (_int)pow(-1, rand())) % 5);
			vRandPos.z = (_float)(((_int)vRandPos.z * (_int)pow(-1, rand())) % 5);
		}*/ // ����... �ϴ� �ּ�

		tagBlackBoardData<Vec3>* pRoamingPoint = new tagBlackBoardData<Vec3>(Vec3(m_pGameObject->GetTransform()->GetPosition()) + vRandPos);
		hashBlackBoard.emplace(TEXT("RoamingPoint"), pRoamingPoint);
	}
	else
	{
		__debugbreak();
	}
}

void CGoblin_BT_Roaming::AbortRoaming()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tRoamingPoint = hashBlackBoard.find(TEXT("RoamingPoint"));

	if (tRoamingPoint != hashBlackBoard.end())
	{
		hashBlackBoard.erase(tRoamingPoint);
	}
}

_bool CGoblin_BT_Roaming::IsAggro()
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

_bool CGoblin_BT_Roaming::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGoblin_BT_Roaming* CGoblin_BT_Roaming::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Roaming* pInstance = new CGoblin_BT_Roaming;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Roaming");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Roaming::Free()
{
	Super::Free();
}