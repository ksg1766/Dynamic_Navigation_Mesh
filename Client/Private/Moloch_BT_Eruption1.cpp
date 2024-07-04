#include "stdafx.h"
#include "Moloch_BT_Eruption1.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "TremorCrystal.h"
#include "Particle.h"
#include "ParticleController.h"

CMoloch_BT_Eruption1::CMoloch_BT_Eruption1()
{
}

void CMoloch_BT_Eruption1::OnStart()
{
	Super::OnStart(0);

	m_bAttack = false;
	m_bCrystalUp = false;
	m_vTargetPos = GetTarget()->GetTransform()->GetPosition();

	m_vecCrystal.clear();
	m_vecCrystal.resize(9);
}

CBT_Node::BT_RETURN CMoloch_BT_Eruption1::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.95f)
		return BT_SUCCESS;

	if (!m_bSoundOn && m_fTimeSum > 0.25f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_crystal_eruption_1.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	if (!m_bCrystalUp)
	{
		if (m_bAttack)
		{
			for (_int i = 0; i < 9; ++i)
				m_vecCrystal[i]->GetTransform()->Translate(2.9f * Vec3::UnitY);

			m_bCrystalUp = true;
		}
	}

	if (!m_bAttack)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f)
		{
			CTransform* pTargetTransform = GetTarget()->GetTransform();

			Vec3 vCreatePosition[9] = {
				m_vTargetPos - 3.3f * Vec3::UnitY,
				m_vTargetPos + 8.f * (pTargetTransform->GetForward() + 0.75f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 8.f * (pTargetTransform->GetForward() - 0.7f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 8.f * (pTargetTransform->GetForward() + 0.7f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 8.f * (pTargetTransform->GetForward() - 0.75f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 6.f * (2.f * pTargetTransform->GetForward()) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 6.f * (2.f * pTargetTransform->GetForward()) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 6.f * (1.5f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 6.f * (1.5f * pTargetTransform->GetRight()) - 3.3f * Vec3::UnitY,
			};

			CTremorCrystal::EFFECT_DESC desc;
			desc.fLifeTime = 4.8f;

			for (_int i = 0; i < 9; ++i)
			{
				desc.fLifeTime += 0.02f;
				m_vecCrystal[i] = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_A"), LAYERTAG::IGNORECOLLISION, &desc);
			}

			for (_int i = 0; i < 9; ++i)
				m_vecCrystal[i]->GetTransform()->Translate(vCreatePosition[i]);

			CParticleController::PARTICLE_DESC tParticleDesc;
			tParticleDesc.vSpeedMax = _float3(4.f, 7.5f, 4.f);
			tParticleDesc.vSpeedMin = _float3(-4.f, 5.0f, -4.f);
			tParticleDesc.fLifeTimeMin = 2.f;
			tParticleDesc.fLifeTimeMax = 2.7f;
			tParticleDesc.fScaleMax = 0.27f;
			tParticleDesc.fScaleMin = 0.15f;

			tParticleDesc.eType = CParticleController::ParticleType::RIGIDBODY;
			for (_int i = 0; i < 9; ++i)
			{
				tParticleDesc.vCenter = m_vecCrystal[i]->GetTransform()->GetPosition();
				for (_int i = 0; i < 5; ++i)
				{
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_G"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_H"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_I"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_L"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_M"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
				}
			}

			tParticleDesc.eType = CParticleController::ParticleType::EXPLODE;
			tParticleDesc.iPass = 1;
			for (_int i = 0; i < 9; ++i)
			{
				tParticleDesc.vCenter = m_vecCrystal[i]->GetTransform()->GetPosition();
				for (_int i = 0; i < 15; ++i)
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			}

			m_bAttack = true;
		}
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Eruption1::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Eruption1::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Eruption1::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Eruption1::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Eruption1* CMoloch_BT_Eruption1::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Eruption1* pInstance = new CMoloch_BT_Eruption1;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Eruption1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Eruption1::Free()
{
	Super::Free();
}