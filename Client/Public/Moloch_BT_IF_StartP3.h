#pragma once
#include "Client_Defines.h"
#include "BT_Decorator.h"
#include "MonsterController.h"

BEGIN(Client)

class CMoloch_BT_IF_StartP3 : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CMoloch_BT_IF_StartP3() = default;
	CMoloch_BT_IF_StartP3(const CMoloch_BT_IF_StartP3& rhs) = delete;
	virtual ~CMoloch_BT_IF_StartP3() = default;

	virtual void OnStart() override
	{

	}

	virtual BT_RETURN OnUpdate(const _float& fTimeDelta) override
	{
		if (!m_IsExecuted)
			return m_vecChildren[0]->Tick(fTimeDelta);
		else
			return BT_FAIL;
	}

	virtual void OnEnd() override
	{
		Reset();
		m_IsExecuted = true;
	}

private:
	_bool m_IsExecuted = false;

public:
	static	CMoloch_BT_IF_StartP3* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CMoloch_BT_IF_StartP3* pInstance = new CMoloch_BT_IF_StartP3;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CMoloch_BT_IF_StartP3");
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