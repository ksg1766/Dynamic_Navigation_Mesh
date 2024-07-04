#include "stdafx.h"
#include "DemonCaster_BT_Dead.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "DissolveManager.h"

CDemonCaster_BT_Dead::CDemonCaster_BT_Dead()
{
}

void CDemonCaster_BT_Dead::OnStart()
{
	Super::OnStart(0);

	m_pGameObject->GetTransform()->RotateYAxisFixed(Vec3(0.f, 180.f, 0.f));
	m_pGameObject->SetInstance(false);
	m_pGameInstance->SwapShader(m_pGameObject, TEXT("Shader_VtxTexFetchAnim"));
	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_demoncaster_death_vo_03.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.2f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CDemonCaster_BT_Dead::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);

	if (!m_bDissolveFlag && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.55f)
	{
		m_pModel->PauseAnimation(true);
		CDissolveManager::GetInstance()->AddDissolve(m_pGameObject);
		m_bDissolveFlag = true;
		//return BT_RUNNING;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
	//return BT_FAIL;
}

void CDemonCaster_BT_Dead::OnEnd()
{
	Super::OnEnd();
}

void CDemonCaster_BT_Dead::ConditionalAbort(const _float& fTimeDelta)
{
}

CDemonCaster_BT_Dead* CDemonCaster_BT_Dead::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDemonCaster_BT_Dead* pInstance = new CDemonCaster_BT_Dead;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDemonCaster_BT_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster_BT_Dead::Free()
{
	Super::Free();
}