#pragma once
#include "Client_Defines.h"
#include "BT_Decorator.h"
#include "MonsterController.h"

BEGIN(Client)

class CHellHound_BT_IF_Dead : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CHellHound_BT_IF_Dead() = default;
	CHellHound_BT_IF_Dead(const CHellHound_BT_IF_Dead& rhs) = delete;
	virtual ~CHellHound_BT_IF_Dead() = default;

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
	static	CHellHound_BT_IF_Dead* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CHellHound_BT_IF_Dead* pInstance = new CHellHound_BT_IF_Dead;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CHellHound_BT_IF_Dead");
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