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
		if (IsInSight())	// 시야에 있는지 없는지 판단해서 키값 삽입or삭제(시야 말고 맞았는지도 추가해야함...)
			return m_vecChildren[0]->Tick(fTimeDelta);
		else				// 키값이 없으면 타겟이 시야에 없는 것
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