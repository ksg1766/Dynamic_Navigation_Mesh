#pragma once
#include "Client_Defines.h"
#include "BT_Decorator.h"
#include "MonsterController.h"

BEGIN(Client)

class CDemonCaster_BT_IF_Dead : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CDemonCaster_BT_IF_Dead() = default;
	CDemonCaster_BT_IF_Dead(const CDemonCaster_BT_IF_Dead& rhs) = delete;
	virtual ~CDemonCaster_BT_IF_Dead() = default;

	virtual void OnStart() override
	{

	}

	virtual BT_RETURN OnUpdate(const _float& fTimeDelta) override
	{
		if (IsZeroHP())
			return m_vecChildren[0]->Tick(fTimeDelta);
		else
			return BT_FAIL;	// 일단 임시로 상태 만들기 전까진 항상 FAIL;
	}

	virtual void OnEnd() override
	{
		Reset();
	}

private:
	_bool	IsZeroHP()
	{
		if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
			return true;

		return false;
	}

public:
	static	CDemonCaster_BT_IF_Dead* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CDemonCaster_BT_IF_Dead* pInstance = new CDemonCaster_BT_IF_Dead;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CDemonCaster_BT_IF_Dead");
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