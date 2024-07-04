#include "stdafx.h"
#include "Moloch_BT_Dash.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "Moloch_MotionTrail.h"
#include "Moloch_SwordSlash.h"

CMoloch_BT_Dash::CMoloch_BT_Dash()
{
}

void CMoloch_BT_Dash::OnStart()
{
	Super::OnStart(0);

	m_bAttack = false;
	m_vTargetPos = GetOrAddTarget()->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(m_vTargetPos);
}

CBT_Node::BT_RETURN CMoloch_BT_Dash::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.95f)
	{
		//pController->GetAttackMessage(0);

		return BT_SUCCESS;
	}

	if (!m_bSoundOn && m_fTimeSum > 0.2f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_dash_strike.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	if (!m_bAttack)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.27f)
		{
			/*CMoloch_SwordSlash::EFFECT_DESC desc;
			CGameObject* pEffect = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Moloch_Sword_Slash"), LAYERTAG::IGNORECOLLISION, &desc);

			Vec3 vFront = -m_pGameObject->GetTransform()->GetForward();
			pEffect->GetTransform()->SetForward(vFront);
			Vec3 vRight = Vec3::UnitY.Cross(vFront);
			pEffect->GetTransform()->SetRight(vRight);
			Vec3 vUp = vFront.Cross(vRight);
			pEffect->GetTransform()->SetUp(vUp);

			pEffect->GetTransform()->SetScale(Vec3(30.f, 30.f, 30.f));

			Vec3 vPos = m_pGameObject->GetTransform()->GetPosition() + 2.f * m_pGameObject->GetTransform()->GetForward() + 1.5f * Vec3::UnitY;
			pEffect->GetTransform()->Translate(vPos);*/

			m_bAttack = true;
		}
	}

	_float fDistance = Vec3::DistanceSquared(m_vTargetPos, m_pGameObject->GetTransform()->GetPosition());
	
	if (m_fTimeSum < m_vecAnimIndexTime[0].second * 0.4f)
	{
		if (fDistance > 4.f)
		{
			CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
			pController->GetDashSpeedMessage();

			m_vTargetPos = GetOrAddTarget()->GetTransform()->GetPosition();
			pController->Look(m_vTargetPos);
			pController->GetTranslateMessage(m_pGameObject->GetTransform()->GetForward());
		}

		if (3 == m_iFrameCounter++)
		{
			CMoloch_MotionTrail::MOTIONTRAIL_DESC desc{ m_pModel, &m_pModel->GetTweenDesc(), m_pGameObject->GetTransform()->WorldMatrix(), 0.18f };
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Moloch_MotionTrail"), LAYERTAG::IGNORECOLLISION, &desc);
			m_iFrameCounter = 0;
		}
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Dash::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Dash::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Dash::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Dash::GetOrAddTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	if (target == hashBlackBoard.end())	// 타겟의 키값이 블랙보드에 없다면(이전에 없었으면 데이터도 없어야 함) 키값 추가해줌.
	{
		map<LAYERTAG, class CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
		const map<LAYERTAG, class CLayer*>::iterator& pPlayerLayer = mapLayers.find(LAYERTAG::PLAYER);
		CGameObject* pPlayer = pPlayerLayer->second->GetGameObjects().front();
		tagBlackBoardData<CGameObject*>* pTarget = new tagBlackBoardData<CGameObject*>(pPlayer);

		hashBlackBoard.emplace(TEXT("Target"), pTarget);
		return pPlayer;
	}
	else
	{
		CGameObject* pPlayer = GET_VALUE(CGameObject, target);
		return pPlayer;
	}
}

CMoloch_BT_Dash* CMoloch_BT_Dash::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Dash* pInstance = new CMoloch_BT_Dash;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Dash::Free()
{
	Super::Free();
}