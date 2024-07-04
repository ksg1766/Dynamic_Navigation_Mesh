#include "stdafx.h"
#include "Goblin_BT_Wait.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CGoblin_BT_Wait::CGoblin_BT_Wait()
{
}

void CGoblin_BT_Wait::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CGoblin_BT_Wait::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.8f)
	{
		return BT_SUCCESS;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CGoblin_BT_Wait::OnEnd()
{
	Super::OnEnd();
}

void CGoblin_BT_Wait::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CGoblin_BT_Wait::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGoblin_BT_Wait* CGoblin_BT_Wait::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Wait* pInstance = new CGoblin_BT_Wait;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Wait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Wait::Free()
{
	Super::Free();
}