#include "stdafx.h"
#include "Moloch_BT_Swing1.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "Moloch_SwordSlash.h"

CMoloch_BT_Swing1::CMoloch_BT_Swing1()
{
}

void CMoloch_BT_Swing1::OnStart()
{
	Super::OnStart(0);

	m_bAttack = false;

	const Vec3& vTargetPos = GetTarget()->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(vTargetPos);
}

CBT_Node::BT_RETURN CMoloch_BT_Swing1::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	/*if (!m_bAttack)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f)
		{
			CMoloch_SwordSlash::EFFECT_DESC desc;
			desc.bCW = true;
			CGameObject* pEffect = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Moloch_Sword_Slash"), LAYERTAG::IGNORECOLLISION, &desc);
			
			Vec3 vFront = m_pGameObject->GetTransform()->GetForward();
			pEffect->GetTransform()->SetForward(vFront);
			Vec3 vRight = Vec3::UnitY.Cross(vFront);
			pEffect->GetTransform()->SetRight(vRight);
			Vec3 vUp = vFront.Cross(vRight);
			pEffect->GetTransform()->SetUp(vUp);
			
			pEffect->GetTransform()->SetScale(Vec3(40.f, 40.f, 40.f));

			pEffect->GetTransform()->RotateYAxisFixed(Vec3(0.f, 90.f, 0.f));

			Vec3 vPos = m_pGameObject->GetTransform()->GetPosition() + 2.f * m_pGameObject->GetTransform()->GetForward() + 1.5f * Vec3::UnitY;
			pEffect->GetTransform()->Translate(vPos);

			m_bAttack = true;
		}
	}*/

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.5f)
	{
		return BT_SUCCESS;
	}

	if (!m_bSoundOn && m_fTimeSum > 0.25f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_full_swing_01.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Swing1::OnEnd()
{
	static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody())->IsKinematic(true);
	Super::OnEnd();
}

void CMoloch_BT_Swing1::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Swing1::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Swing1::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Swing1* CMoloch_BT_Swing1::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Swing1* pInstance = new CMoloch_BT_Swing1;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Swing1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Swing1::Free()
{
	Super::Free();
}