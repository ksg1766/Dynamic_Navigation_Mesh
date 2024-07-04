#include "stdafx.h"
#include "Moloch_BT_Swing3.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

CMoloch_BT_Swing3::CMoloch_BT_Swing3()
{
}

void CMoloch_BT_Swing3::OnStart()
{
	Super::OnStart(0);

	const Vec3& vTargetPos = GetTarget()->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(vTargetPos);

	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_full_swing_03.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CMoloch_BT_Swing3::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.97f)
	{
		return BT_SUCCESS;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Swing3::OnEnd()
{
	static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody())->IsKinematic(true);
	Super::OnEnd();
}

void CMoloch_BT_Swing3::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Swing3::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Swing3::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Swing3* CMoloch_BT_Swing3::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Swing3* pInstance = new CMoloch_BT_Swing3;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Swing3");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Swing3::Free()
{
	Super::Free();
}