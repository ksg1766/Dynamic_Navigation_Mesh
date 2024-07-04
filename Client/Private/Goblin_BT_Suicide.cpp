#include "stdafx.h"
#include "Goblin_BT_Suicide.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterController.h"

CGoblin_BT_Suicide::CGoblin_BT_Suicide()
{
}

void CGoblin_BT_Suicide::OnStart()
{
	Super::OnStart(0);

	switch (rand() % 3)
	{
	case 0:
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_build_01.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.3f)))
			__debugbreak();
		break;
	case 1:
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_build_02.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.3f)))
			__debugbreak();
		break;
	case 2:
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_build_03.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.3f)))
			__debugbreak();
		break;
	}
}

CBT_Node::BT_RETURN CGoblin_BT_Suicide::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
		pController->GetAttackMessage();
		
		m_pGameInstance->DeleteObject(m_pGameObject);

		_int iRandom = rand() % 3;
		switch (iRandom)
		{
		case 0:
			if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_01.ogg"), (CHANNELID)((_int)CHANNELID::CHANNEL_30 - iRandom), 0.5f)))
				__debugbreak();
			break;
		case 1:
			if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_02.ogg"), (CHANNELID)((_int)CHANNELID::CHANNEL_30 - iRandom), 0.5f)))
				__debugbreak();
			break;
		case 2:
			if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_explode_03.ogg"), (CHANNELID)((_int)CHANNELID::CHANNEL_30 - iRandom), 0.5f)))
				__debugbreak();
			break;
		}

		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bubble"), LAYERTAG::UNIT_AIR)
			->GetTransform()->Translate(m_pGameObject->GetTransform()->GetPosition());
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Wave_Ring"), LAYERTAG::IGNORECOLLISION)
			->GetTransform()->Translate(m_pGameObject->GetTransform()->GetPosition());

		return BT_SUCCESS;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CGoblin_BT_Suicide::OnEnd()
{
	Super::OnEnd();
}

void CGoblin_BT_Suicide::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CGoblin_BT_Suicide::IsZeroHP()
{
	if(static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CGoblin_BT_Suicide* CGoblin_BT_Suicide::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Suicide* pInstance = new CGoblin_BT_Suicide;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Suicide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Suicide::Free()
{
	Super::Free();
}