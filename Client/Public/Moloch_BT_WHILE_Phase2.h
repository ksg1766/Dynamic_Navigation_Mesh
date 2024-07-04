#pragma once
#include "Client_Defines.h"
#include "BT_Decorator.h"
#include "MonsterController.h"
#include "MonsterStats.h"

BEGIN(Client)

class CMoloch_BT_WHILE_Phase2 : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CMoloch_BT_WHILE_Phase2() = default;
	CMoloch_BT_WHILE_Phase2(const CMoloch_BT_WHILE_Phase2& rhs) = delete;
	virtual ~CMoloch_BT_WHILE_Phase2() = default;

	virtual void OnStart() override
	{

	}

	virtual BT_RETURN OnUpdate(const _float& fTimeDelta) override
	{
		if (IsPhaseEnd())
			return BT_FAIL;
		else
			return m_vecChildren[0]->Tick(fTimeDelta);
	}

	virtual void OnEnd() override
	{
		Reset();
	}

private:
	_bool	IsPhaseEnd()
	{
		if (static_cast<CMonsterController*>(m_pController)->GetStats()->GetHP() < 1000)
			return true;

		return false;
	}

public:
	static	CMoloch_BT_WHILE_Phase2* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CMoloch_BT_WHILE_Phase2* pInstance = new CMoloch_BT_WHILE_Phase2;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CMoloch_BT_WHILE_Phase2");
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