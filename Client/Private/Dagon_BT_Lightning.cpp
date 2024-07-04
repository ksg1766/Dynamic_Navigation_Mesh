#include "stdafx.h"
#include "Dagon_BT_Lightning.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossController.h"

CDagon_BT_Lightning::CDagon_BT_Lightning()
{
}

void CDagon_BT_Lightning::OnStart()
{
	Super::OnStart(0);
	HitOrMiss();
	m_bAttacked = false;
}

CBT_Node::BT_RETURN CDagon_BT_Lightning::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (!m_bHitOrMiss)
		return BT_FAIL;

	if (0 == m_fTimeSum)
	{
		_int bResult;
		m_bSoundIndex ? bResult = m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_calllightning_01.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)
			: bResult = m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_calllightning_02.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f);

		if (FAILED(bResult)) __debugbreak();
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		return BT_SUCCESS;
	}

	if (!m_bAttacked && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.5f)
	{
		vector<CGameObject*> vecWaterLightning;
		vector<CGameObject*> vecBolts;
		vecWaterLightning.resize(5);
		vecBolts.resize(5);
		for (_int i = 0; i < 5; ++i)
		{
			CGameObject* pWaterLightning = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_WaterLightning"), LAYERTAG::UNIT_AIR);
			vecWaterLightning[i] = pWaterLightning;
			pWaterLightning->GetTransform()->SetScale(Vec3(3.7f, 1.5f, 3.7f));
			pWaterLightning->GetTransform()->SetPosition(m_pGameObject->GetTransform()->GetPosition() + 2.2f * Vec3::UnitY);

			CGameObject* pBolts = nullptr;

			static _int iBoltNum = 0;

			if (0 == iBoltNum)
				pBolts = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bolts0"), LAYERTAG::IGNORECOLLISION);
			if (1 == iBoltNum)
				pBolts = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bolts1"), LAYERTAG::IGNORECOLLISION);
			if (2 == iBoltNum)
				pBolts = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bolts2"), LAYERTAG::IGNORECOLLISION);
			if (3 == iBoltNum)
				pBolts = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bolts3"), LAYERTAG::IGNORECOLLISION);

			if (3 == iBoltNum)
				iBoltNum = 0;
			else
				++iBoltNum;

			vecBolts[i] = pBolts;

			pBolts->GetTransform()->Rotate(Vec3(rand() % 360, rand() % 360, rand() % 360));
			pBolts->GetTransform()->SetPosition(m_pGameObject->GetTransform()->GetPosition() + 3.2f * Vec3::UnitY);
		}

		_int iOffset = (rand() % 10) - 5;

		vecWaterLightning[0]->GetTransform()->Translate(Vec3(-7.5f, 0.f, -27.5f - 0.33f * iOffset));
		vecWaterLightning[1]->GetTransform()->Translate(Vec3(7.5f, 0.f, -27.5f));
		vecWaterLightning[2]->GetTransform()->Translate(Vec3(-7.5f - 0.2f * iOffset, 0.f, -13.f + iOffset));
		vecWaterLightning[3]->GetTransform()->Translate(Vec3(7.5f, 0.f, -13.f + 0.5f * iOffset));
		vecWaterLightning[4]->GetTransform()->Translate(Vec3(0.f, 0.f, -20.f));
		vecBolts[0]->GetTransform()->Translate(Vec3(-7.5f, 0.f, -27.5f - 0.33f * iOffset));
		vecBolts[1]->GetTransform()->Translate(Vec3(7.5f, 0.f, -27.5f));
		vecBolts[2]->GetTransform()->Translate(Vec3(-7.5f - 0.2f * iOffset, 0.f, -13.f + iOffset));
		vecBolts[3]->GetTransform()->Translate(Vec3(7.5f, 0.f, -13.f + 0.5f * iOffset));
		vecBolts[4]->GetTransform()->Translate(Vec3(0.f, 0.f, -20.f));

		m_bAttacked = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CDagon_BT_Lightning::OnEnd()
{
	Super::OnEnd();
}

void CDagon_BT_Lightning::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CDagon_BT_Lightning::IsZeroHP()
{
	if (static_cast<CBossController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

void CDagon_BT_Lightning::HitOrMiss()
{
	random_device		RandomDevice;

	mt19937_64							RandomNumber(RandomDevice());
	uniform_int_distribution<_int>		iRandom(0, 20);

	if (6 >= iRandom(RandomNumber))
		m_bHitOrMiss = true;
	else
		m_bHitOrMiss = false;
}

CDagon_BT_Lightning* CDagon_BT_Lightning::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDagon_BT_Lightning* pInstance = new CDagon_BT_Lightning;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDagon_BT_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagon_BT_Lightning::Free()
{
	Super::Free();
}