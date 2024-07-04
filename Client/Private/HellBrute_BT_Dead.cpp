#include "stdafx.h"
#include "HellBrute_BT_Dead.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "DissolveManager.h"

CHellBrute_BT_Dead::CHellBrute_BT_Dead()
{
}

void CHellBrute_BT_Dead::OnStart()
{
	Super::OnStart(0);

	m_pGameObject->SetInstance(false);
	m_pGameInstance->SwapShader(m_pGameObject, TEXT("Shader_VtxTexFetchAnim"));

	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellbrute_death_interactive.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.3f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CHellBrute_BT_Dead::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);

	if (!m_bDissolveFlag && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.8f)
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

void CHellBrute_BT_Dead::OnEnd()
{
	Super::OnEnd();
}

void CHellBrute_BT_Dead::ConditionalAbort(const _float& fTimeDelta)
{
}

CHellBrute_BT_Dead* CHellBrute_BT_Dead::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellBrute_BT_Dead* pInstance = new CHellBrute_BT_Dead;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellBrute_BT_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute_BT_Dead::Free()
{
	Super::Free();
}