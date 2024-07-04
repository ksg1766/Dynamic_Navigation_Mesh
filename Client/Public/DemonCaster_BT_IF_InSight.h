#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "BT_Decorator.h"
#include "Layer.h"

BEGIN(Client)

class CDemonCaster_BT_IF_InSight : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CDemonCaster_BT_IF_InSight() = default;
	CDemonCaster_BT_IF_InSight(const CDemonCaster_BT_IF_InSight& rhs) = delete;
	virtual ~CDemonCaster_BT_IF_InSight() = default;

	virtual void		OnStart() override
	{

	}

	virtual BT_RETURN	OnUpdate(const _float& fTimeDelta) override
	{
		if (IsInSight())	// �þ߿� �ִ��� ������ �Ǵ��ؼ� Ű�� ����or����(�þ� ���� �¾Ҵ����� �߰��ؾ���...)
			return m_vecChildren[0]->Tick(fTimeDelta);
		else				// Ű���� ������ Ÿ���� �þ߿� ���� ��
			return BT_FAIL;
	}

	virtual void		OnEnd() override
	{
		Reset();
	}

private:
	_bool	IsInSight()
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

public:
	static	CDemonCaster_BT_IF_InSight* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CDemonCaster_BT_IF_InSight* pInstance = new CDemonCaster_BT_IF_InSight;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CDemonCaster_BT_IF_InSight");
			Safe_Release(pInstance);
		}

		return pInstance;
	}
	virtual void Free() override
	{
		Super::Free();
	}
};

END