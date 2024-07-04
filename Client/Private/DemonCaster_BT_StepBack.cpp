#include "stdafx.h"
#include "DemonCaster_BT_StepBack.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CDemonCaster_BT_StepBack::CDemonCaster_BT_StepBack()
{
}

void CDemonCaster_BT_StepBack::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CDemonCaster_BT_StepBack::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (!IsInRange())
	{
		return BT_FAIL;
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.8f)
	{
		return BT_SUCCESS;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CDemonCaster_BT_StepBack::OnEnd()
{
	Super::OnEnd();
}

void CDemonCaster_BT_StepBack::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CDemonCaster_BT_StepBack::IsInRange()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& meleeRange = hashBlackBoard.find(TEXT("MeleeRange"));
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
	const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();
	Vec3 vDist = vTargetPos - vCurPos;

	if (vDist.Length() < *GET_VALUE(_float, meleeRange))
		return true;
	else
		return false;
}

_bool CDemonCaster_BT_StepBack::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CDemonCaster_BT_StepBack* CDemonCaster_BT_StepBack::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDemonCaster_BT_StepBack* pInstance = new CDemonCaster_BT_StepBack;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDemonCaster_BT_StepBack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster_BT_StepBack::Free()
{
	Super::Free();
}