#include "stdafx.h"
#include "Moloch_BT_Geyser1.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"
#include "TremorCrystal.h"
#include "Particle.h"
#include "CrystalParticle.h"
#include "ParticleController.h"

CMoloch_BT_Geyser1::CMoloch_BT_Geyser1()
{
}

void CMoloch_BT_Geyser1::OnStart()
{
	Super::OnStart(0);

	m_bAttack = false;
	m_bCrystalUp = false;


	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& isSecondRun = hashBlackBoard.find(TEXT("GeyserSecondRun"));

	if (hashBlackBoard.end() == isSecondRun)
	{
		m_vTargetPos = GetTarget()->GetTransform()->GetPosition();
		m_vTargetForward = GetTarget()->GetTransform()->GetForward();
		m_vTargetRight = GetTarget()->GetTransform()->GetRight();
		m_vecCrystal.clear();
		m_vecCrystal.resize(13);
	}
}

CBT_Node::BT_RETURN CMoloch_BT_Geyser1::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& isSecondRun = hashBlackBoard.find(TEXT("GeyserSecondRun"));

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.97f)
	{
		if (hashBlackBoard.end() == isSecondRun)
			hashBlackBoard.emplace(TEXT("GeyserSecondRun"), new tagBlackBoardData<_bool>(true));
		else
		{
			_float fLifeTime = 2.f;
			for (_int i = 0; i < 13; ++i)
			{
				static_cast<CTremorCrystal*>(m_vecCrystal[i])->SetLightning();
				static_cast<CTremorCrystal*>(m_vecCrystal[i])->SetLifeTime(fLifeTime + 0.02f * i);
			}
			hashBlackBoard.erase(isSecondRun);
		}

		return BT_SUCCESS;
	}

	if (!m_bSoundOn && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.15f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_atk_full_geyser_1.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
			__debugbreak();

		m_bSoundOn = true;
	}

	if(hashBlackBoard.end() == isSecondRun)
		FirstRun();
	else
		SecondRun();

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CMoloch_BT_Geyser1::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Geyser1::FirstRun()
{
	if (!m_bCrystalUp)
	{
		if (m_bAttack)
		{
			for (_int i = 0; i < 7; ++i)
				m_vecCrystal[i]->GetTransform()->Translate(2.9f * Vec3::UnitY);

			m_bCrystalUp = true;
		}
	}

	if (!m_bAttack)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f)
		{
			Vec3 vCreatePosition[7] = {
				m_vTargetPos - 3.3f * Vec3::UnitY,
				m_vTargetPos + 7.7f * (1.2f * m_vTargetForward + 0.7f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 7.7f * (1.2f * m_vTargetForward - 0.7f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 7.7f * (1.2f * m_vTargetForward + 0.7f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 7.7f * (1.2f * m_vTargetForward - 0.7f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 7.5f * (2.f * m_vTargetForward) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 7.5f * (2.f * m_vTargetForward) - 3.3f * Vec3::UnitY,
			};

			CTremorCrystal::EFFECT_DESC desc;
			desc.fLifeTime = D3D11_FLOAT32_MAX;

			for (_int i = 0; i < 7; ++i)
			{
				m_vecCrystal[i] = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_A"), LAYERTAG::IGNORECOLLISION, &desc);
			}

			for (_int i = 0; i < 7; ++i)
			{
				m_vecCrystal[i]->GetTransform()->Translate(vCreatePosition[i]);
			}
			CParticleController::PARTICLE_DESC tParticleDesc;
			tParticleDesc.vSpeedMax = _float3(4.f, 7.5f, 4.f);
			tParticleDesc.vSpeedMin = _float3(-4.f, 5.0f, -4.f);
			tParticleDesc.fLifeTimeMin = 2.f;
			tParticleDesc.fLifeTimeMax = 2.7f;
			tParticleDesc.fScaleMax = 0.3f;
			tParticleDesc.fScaleMin = 0.17f;

			tParticleDesc.eType = CParticleController::ParticleType::RIGIDBODY;
			for (_int i = 0; i < 7; ++i)
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
			for (_int i = 0; i < 7; ++i)
			{
				tParticleDesc.vCenter = m_vecCrystal[i]->GetTransform()->GetPosition();
				for (_int i = 0; i < 15; ++i)
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			}

			

			m_bAttack = true;
		}
	}
}

void CMoloch_BT_Geyser1::SecondRun()
{
	if (!m_bCrystalUp)
	{
		if (m_bAttack)
		{
			for (_int i = 7; i < 13; ++i)
				m_vecCrystal[i]->GetTransform()->Translate(2.9f * Vec3::UnitY);

			m_bCrystalUp = true;
		}
	}

	if (!m_bAttack)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.2f)
		{
			CTransform* pTargetTransform = GetTarget()->GetTransform();

			Vec3 vCreatePosition[6] = {
				m_vTargetPos + 7.f * (0.7f * m_vTargetForward + 1.2f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 7.f * (0.7f * m_vTargetForward - 1.2f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 7.f * (0.7f * m_vTargetForward + 1.2f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 7.f * (0.7f * m_vTargetForward - 1.2f * m_vTargetRight) - 3.3f * Vec3::UnitY,
				m_vTargetPos + 5.5f * (1.f * m_vTargetForward) - 3.3f * Vec3::UnitY,
				m_vTargetPos - 5.5f * (1.f * m_vTargetForward) - 3.3f * Vec3::UnitY,
			};

			CTremorCrystal::EFFECT_DESC desc;
			desc.fLifeTime = D3D11_FLOAT32_MAX;

			for (_int i = 7; i < 13; ++i)
			{
				m_vecCrystal[i] = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_A"), LAYERTAG::IGNORECOLLISION, &desc);
				static_cast<CTremorCrystal*>(m_vecCrystal[i])->SetLightning();
			}

			for (_int i = 7; i < 13; ++i)
				m_vecCrystal[i]->GetTransform()->Translate(vCreatePosition[i - 7]);

			CParticleController::PARTICLE_DESC tParticleDesc;
			tParticleDesc.eType = CParticleController::ParticleType::RIGIDBODY;
			tParticleDesc.vSpeedMax = _float3(4.f, 7.5f, 4.f);
			tParticleDesc.vSpeedMin = _float3(-4.f, 5.0f, -4.f);
			tParticleDesc.fLifeTimeMin = 2.f;
			tParticleDesc.fLifeTimeMax = 2.7f;
			tParticleDesc.fScaleMax = 0.3f;
			tParticleDesc.fScaleMin = 0.17f;

			for (_int i = 7; i < 13; ++i)
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
			for (_int i = 7; i < 13; ++i)
			{
				tParticleDesc.vCenter = m_vecCrystal[i]->GetTransform()->GetPosition();
				for (_int i = 0; i < 15; ++i)
					m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			}

			m_bAttack = true;
		}
	}
}

void CMoloch_BT_Geyser1::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CMoloch_BT_Geyser1::IsZeroHP()
{
	if (static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGameObject* CMoloch_BT_Geyser1::GetTarget()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));
	CGameObject* pPlayer = GET_VALUE(CGameObject, target);

	return pPlayer;
}

CMoloch_BT_Geyser1* CMoloch_BT_Geyser1::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Geyser1* pInstance = new CMoloch_BT_Geyser1;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Geyser1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Geyser1::Free()
{
	Super::Free();
}