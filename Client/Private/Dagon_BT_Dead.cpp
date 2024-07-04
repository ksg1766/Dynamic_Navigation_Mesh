#include "stdafx.h"
#include "Dagon_BT_Dead.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "DissolveManager.h"
#include "MonsterController.h"
#include "MainCameraController.h"

#include "MainCamera.h"
#include "MainCameraController.h"
#include "Layer.h"
#include "Water.h"

CDagon_BT_Dead::CDagon_BT_Dead()
{
}

void CDagon_BT_Dead::OnStart()
{
	Super::OnStart(0);

	m_pGameObject->SetInstance(false);
	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_stun.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.5f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CDagon_BT_Dead::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);

	m_pGameInstance->Get_LightDesc(0)->vDiffuse = _float4(std::min(0.5f + m_fTimeSum, 1.f), std::min(0.5f + m_fTimeSum, 1.f), std::min(0.5f + m_fTimeSum, 1.f), 1.f);

	if (!m_bDissolveFlag && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.92f)
	{
		m_pModel->PauseAnimation(true);
		CDissolveManager::GetInstance()->AddDissolve(m_pGameObject);
		m_bDissolveFlag = true;

		m_pGameInstance->Get_LightDesc(0)->vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);

		static_cast<CMonsterController*>(m_pController)->StopRain();

		CMainCamera* pCamera = static_cast<CMainCamera*>(m_pGameInstance->GetCurrentCamera());
		CMainCameraController* pController = pCamera->GetController();
		pController->SetCameraMode(CMainCameraController::CameraMode::DagonToBase);

		// WaterLevel
		map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
		const auto& pair = mapLayers.find(LAYERTAG::TERRAIN);

		CWater* pWater = nullptr;
		for (auto& iter : pair->second->GetGameObjects())
		{
			if (TEXT("Water") == iter->GetObjectTag())
			{
				pWater = static_cast<CWater*>(iter);
				break;
			}
		}

		if (nullptr != pWater)
			pWater->SetMode(CWater::WaterLevelMode::Dessert);
		//return BT_RUNNING;

		if (FAILED(m_pGameInstance->StopSound(CHANNELID::CHANNEL_AMBIENT0)) ||
			FAILED(m_pGameInstance->StopSound(CHANNELID::CHANNEL_AMBIENT1)))
			__debugbreak();
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
	//return BT_FAIL;
}

void CDagon_BT_Dead::OnEnd()
{
	Super::OnEnd();
}

void CDagon_BT_Dead::ConditionalAbort(const _float& fTimeDelta)
{
}

CDagon_BT_Dead* CDagon_BT_Dead::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDagon_BT_Dead* pInstance = new CDagon_BT_Dead;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDagon_BT_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagon_BT_Dead::Free()
{
	Super::Free();
}