#include "stdafx.h"
#include "Goblin_BT_Dead.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "DissolveManager.h"

CGoblin_BT_Dead::CGoblin_BT_Dead()
{
}

void CGoblin_BT_Dead::OnStart()
{
	Super::OnStart(0);

	m_pGameObject->SetInstance(false);
	m_pGameInstance->SwapShader(m_pGameObject, TEXT("Shader_VtxTexFetchAnim"));

	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_fleamag_scream_short_07.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.2f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CGoblin_BT_Dead::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);

	if (!m_bDissolveFlag && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.8f)
	{
		m_pModel->PauseAnimation(true);
		CDissolveManager::GetInstance()->AddDissolve(m_pGameObject);
		m_bDissolveFlag = true;

		//m_pGameInstance->DeleteObject(m_pGameObject);
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CGoblin_BT_Dead::OnEnd()
{
	Super::OnEnd();
}

void CGoblin_BT_Dead::ConditionalAbort(const _float& fTimeDelta)
{
}

CGoblin_BT_Dead* CGoblin_BT_Dead::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CGoblin_BT_Dead* pInstance = new CGoblin_BT_Dead;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CGoblin_BT_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_BT_Dead::Free()
{
	Super::Free();
}