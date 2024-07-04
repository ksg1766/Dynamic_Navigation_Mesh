#include "stdafx.h"
#include "Dagon_BT_Wave.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossController.h"

CDagon_BT_Wave::CDagon_BT_Wave()
{
}

void CDagon_BT_Wave::OnStart()
{
	Super::OnStart(0);
	m_bAttacked = false;
}

CBT_Node::BT_RETURN CDagon_BT_Wave::OnUpdate(const _float& fTimeDelta)
{
	if (IsZeroHP())
		return BT_FAIL;

	if (0.f == m_fTimeSum)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_tidalwave_start.ogg"), CHANNELID::CHANNEL_ENEMY0, 0.7f)) ||
			FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_tidalwave.ogg"), CHANNELID::CHANNEL_ENEMY1, 0.7f)))
			__debugbreak();
	}

	if (m_fTimeSum > m_vecAnimIndexTime[0].second * 0.9f)
	{
		return BT_SUCCESS;
	}

	if (!m_bAttacked && m_fTimeSum > m_vecAnimIndexTime[0].second * 0.3f)
	{
		CGameObject* pDagonWave = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_DagonWave"), LAYERTAG::IGNORECOLLISION);
		const Vec3& vForward = m_pGameObject->GetTransform()->GetForward();

		pDagonWave->GetTransform()->SetForward(const_cast<Vec3&>(vForward));
		Vec3 vNewRight = Vec3::UnitY.Cross(vForward);
		pDagonWave->GetTransform()->SetRight(vNewRight);
		Vec3 vNewUp = vForward.Cross(vNewRight);
		pDagonWave->GetTransform()->SetUp(vNewUp);

		pDagonWave->GetTransform()->SetPosition(m_pGameObject->GetTransform()->GetPosition());
		m_bAttacked = true;
	}

	m_fTimeSum += fTimeDelta;

	return BT_RUNNING;
}

void CDagon_BT_Wave::OnEnd()
{
	Super::OnEnd();
}

void CDagon_BT_Wave::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CDagon_BT_Wave::IsZeroHP()
{
	if (static_cast<CBossController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

CDagon_BT_Wave* CDagon_BT_Wave::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CDagon_BT_Wave* pInstance = new CDagon_BT_Wave;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CDagon_BT_Wave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagon_BT_Wave::Free()
{
	Super::Free();
}