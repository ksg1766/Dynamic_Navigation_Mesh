#include "stdafx.h"
#include "Moloch_BT_Dead.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "DissolveManager.h"

CMoloch_BT_Dead::CMoloch_BT_Dead()
{
}

void CMoloch_BT_Dead::OnStart()
{
	Super::OnStart(0);

	m_pGameObject->SetInstance(false);
	m_pGameInstance->SwapShader(m_pGameObject, TEXT("Shader_VtxTexFetchAnim"));
	if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_moloch_impact_stun.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.5f)))
		__debugbreak();
}

CBT_Node::BT_RETURN CMoloch_BT_Dead::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);

	if (!m_bDissolveFlag && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.92f)
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

void CMoloch_BT_Dead::OnEnd()
{
	Super::OnEnd();
}

void CMoloch_BT_Dead::ConditionalAbort(const _float& fTimeDelta)
{
}

CMoloch_BT_Dead* CMoloch_BT_Dead::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CMoloch_BT_Dead* pInstance = new CMoloch_BT_Dead;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CMoloch_BT_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_BT_Dead::Free()
{
	Super::Free();
}