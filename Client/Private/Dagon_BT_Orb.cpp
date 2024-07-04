#include "stdafx.h"
#include "Dagon_BT_Orb.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossController.h"

_int CDagon_BT_Orb::m_iOrbNum = 0;
_bool CDagon_BT_Orb::m_isOrbLeft = false;

CDagon_BT_Orb::CDagon_BT_Orb()
{
}

void CDagon_BT_Orb::OnStart()
{
	Super::OnStart(0);
	HitOrMiss();
	m_bAttacked = false;

	if (2 == m_iOrbNum)
	{
		m_iOrbNum = -1;
		m_isOrbLeft = false;
	}
}

CBT_Node::BT_RETURN CDagon_BT_Orb::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (!m_bHitOrMiss)
		return BT_FAIL;

	if (-1 == m_iOrbNum)
	{
		m_iOrbNum = 0;
		return BT_FAIL;
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		return BT_SUCCESS;
	}

	if (!m_bAttacked && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.3f)
	{
		_int iLeftRight = 0;
		if (m_isOrbLeft)
		{
			iLeftRight = 1;

			if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_summon_orb_start_01.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
				__debugbreak();
		}
		else
		{
			iLeftRight = -1;
			m_isOrbLeft = true;

			if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_summon_orb_start_02.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)))
				__debugbreak();
		}

		CGameObject* pOrb1 = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Orb"), LAYERTAG::IGNORECOLLISION);
		pOrb1->GetTransform()->SetPosition(m_pGameObject->GetTransform()->GetPosition() + 3.f * m_pGameObject->GetTransform()->GetForward() +
			5.f * iLeftRight * m_pGameObject->GetTransform()->GetRight() + 2.f * Vec3::UnitY);

		++m_iOrbNum;
		m_bAttacked = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CDagon_BT_Orb::OnEnd()
{
	Super::OnEnd();
}

void CDagon_BT_Orb::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CDagon_BT_Orb::IsZeroHP()
{
	if (static_cast<CBossController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

void CDagon_BT_Orb::HitOrMiss()
{
	random_device		RandomDevice;

	mt19937_64							RandomNumber(RandomDevice());
	uniform_int_distribution<_int>		iRandom(0, 20);

	if (6 >= iRandom(RandomNumber))
		m_bHitOrMiss = true;
	else
		m_bHitOrMiss = false;
}

CDagon_BT_Orb* CDagon_BT_Orb::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDagon_BT_Orb* pInstance = new CDagon_BT_Orb;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDagon_BT_Orb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagon_BT_Orb::Free()
{
	Super::Free();
}