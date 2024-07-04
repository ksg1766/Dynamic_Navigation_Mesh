#include "stdafx.h"
#include "Moloch_BT_FullDash2.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "TremorCrystal.h"
#include "MonsterController.h"
#include "Moloch_MotionTrail.h"
#include "Particle.h"
#include "ParticleController.h"

CMoloch_BT_FullDash2::CMoloch_BT_FullDash2()
{
}

void CMoloch_BT_FullDash2::OnStart()
{
	Super::OnStart(0);

	m_vTargetPos = GetTarget()->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(m_vTargetPos);

	m_iCrystalCounter = 24;
}

CBT_Node::BT_RETURN CMoloch_BT_FullDash2::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.97f)
	{
		//CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
		//pController->GetAttackMessage(0);

		return BT_SUCCESS;
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.5f)
	{
		if (0 <= m_iCrystalCounter && 0 == m_iCrystalCounter-- % 4)
		{
			CTremorCrystal::EFFECT_DESC desc;
			desc.fLifeTime = 7.f;

			CTremorCrystal* pCrystal = static_cast<CTremorCrystal*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_A"), LAYERTAG::IGNORECOLLISION, &desc));
			const Vec3& vForward = m_pGameObject->GetTransform()->GetForward();

			pCrystal->GetTransform()->Translate(m_pGameObject->GetTransform()->GetPosition() + Vec3(0.f, -0.125f * (_float)m_iCrystalCounter, 0.f) + 2.9f * (6.4f - 0.25f * (_float)m_iCrystalCounter) * vForward);
			pCrystal->SetLightning();

			CParticleController::PARTICLE_DESC tParticleDesc;
			tParticleDesc.eType = CParticleController::ParticleType::RIGIDBODY;
			tParticleDesc.vSpeedMax = _float3(4.f, 7.5f, 4.f);
			tParticleDesc.vSpeedMin = _float3(-4.f, 5.f, -4.f);
			tParticleDesc.fLifeTimeMin = 2.f;
			tParticleDesc.fLifeTimeMax = 2.7f;
			tParticleDesc.fScaleMax = 0.3f;
			tParticleDesc.fScaleMin = 0.15f;
			tParticleDesc.vCenter = pCrystal->GetTransform()->GetPosition();

			for (_int i = 0; i < 5; ++i)
			{
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_G"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_H"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_I"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_L"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_M"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			}

			tParticleDesc.eType = CParticleController::ParticleType::EXPLODE;
			tParticleDesc.iPass = 1;
			for (_int i = 0; i < 15; ++i)
				m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
		}
	}

	_float fDistance = Vec3::DistanceSquared(m_vTargetPos, m_pGameObject->GetTransform()->GetPosition());
	if (m_fTimeSum < m_vecAnimIndexTime[0].second * 0.4f)
	{
		if (fDistance > 4.f)
		{
			CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
			pController->GetDashSpeedMessage();

			m_vTargetPos = GetTarget()->GetTransform()->GetPosition();
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

	if (!m_bSoundOn && m_fTimeSum < m_vecAnimIndexTime[0].second * 0.1f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_full_dash_strike_2.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_FullDash2::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_FullDash2::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_FullDash2::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_FullDash2::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_FullDash2* CMoloch_BT_FullDash2::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_FullDash2* pInstance = new CMoloch_BT_FullDash2;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_FullDash2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_FullDash2::Free()
{
	Super::Free();
}