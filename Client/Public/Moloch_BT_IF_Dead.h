#pragma once
#include "Client_Defines.h"
#include "BT_Decorator.h"
#include "MonsterController.h"

BEGIN(Client)

class CMoloch_BT_IF_Dead : public CBT_Decorator
{
	using Super = CBT_Decorator;
private:
	CMoloch_BT_IF_Dead() = default;
	CMoloch_BT_IF_Dead(const CMoloch_BT_IF_Dead& rhs) = delete;
	virtual ~CMoloch_BT_IF_Dead() = default;

	virtual void OnStart() override
	{

	}

	virtual BT_RETURN OnUpdate(const _float& fTimeDelta) override
	{
		if (IsZeroHP())
			return m_vecChildren[0]->Tick(fTimeDelta);
		else
			return BT_FAIL;
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
	static	CMoloch_BT_IF_Dead* Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, CMonoBehaviour* pController, DecoratorType eDecoratorType)
	{
		CMoloch_BT_IF_Dead* pInstance = new CMoloch_BT_IF_Dead;

		if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, pController, eDecoratorType)))
		{
			MSG_BOX("Failed to Created : CMoloch_BT_IF_Dead");
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