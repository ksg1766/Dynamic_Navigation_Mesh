#include "stdafx.h"
#include "HellBrute_BT_Fire.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "MonsterController.h"

#include "Strife_Ammo_Default.h"

CHellBrute_BT_Fire::CHellBrute_BT_Fire()
{
}

void CHellBrute_BT_Fire::OnStart()
{
	Super::OnStart(0);
}

CBT_Node::BT_RETURN CHellBrute_BT_Fire::OnUpdate(const _float& fTimeDelta)
{
	ConditionalAbort(fTimeDelta);
	if (IsZeroHP())
		return BT_FAIL;

	if (IsInRange())
		return BT_FAIL;

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();

	CMonsterController* pController = static_cast<CMonsterController*>(m_pController);
	pController->Look(vTargetPos);
	m_pGameObject->GetTransform()->RotateYAxisFixed(Vec3(0.f, -3.f, 0.f));

	m_fFR_Default_Timer -= fTimeDelta;
	if (m_fFR_Default_Timer < 0.f)
	{
		Fire();
		m_fFR_Default_Timer = m_fFR_Default;

		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_hellbrute_gatling_gun_loop.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.2f)))
			__debugbreak();
	}
	//pController->GetAttackMessage();

	return BT_RUNNING;
}

void CHellBrute_BT_Fire::OnEnd()
{
	Super::OnEnd();
}

void CHellBrute_BT_Fire::ConditionalAbort(const _float& fTimeDelta)
{
}

_bool CHellBrute_BT_Fire::IsInRange()
{
	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& meleeRange = hashBlackBoard.find(TEXT("MeleeRange"));
	const auto& target = hashBlackBoard.find(TEXT("Target"));

	const Vec3& vTargetPos = GET_VALUE(CGameObject, target)->GetTransform()->GetPosition();
	const Vec3& vCurPos = m_pGameObject->GetTransform()->GetPosition();
	Vec3 vDist = vTargetPos - vCurPos;

	if (vDist.Length() < *GET_VALUE(_float, meleeRange))
	{
		if (FAILED(m_pGameInstance->StopSound(CHANNELID::CHANNEL_ENEMY2)))
			__debugbreak();
		return true;
	}
	else
		return false;
}

_bool CHellBrute_BT_Fire::IsInSight()
{
	map<LAYERTAG, class CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	const map<LAYERTAG, class CLayer*>::iterator& pPlayerLayer = mapLayers.find(LAYERTAG::PLAYER);

	BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
	const auto& tSight = hashBlackBoard.find(TEXT("Sight"));
	const auto& tTarget = hashBlackBoard.find(TEXT("Target"));

	CGameObject* pPlayer = pPlayerLayer->second->GetGameObjects().front();
	if ((pPlayer->GetTransform()->GetPosition() - m_pGameObject->GetTransform()->GetPosition()).Length() < *GET_VALUE(_float, tSight))	// 시야에 있다면
	{
		if (tTarget == hashBlackBoard.end())	// 타겟의 키값이 블랙보드에 없다면(이전에 없었으면 데이터도 없어야 함) 키값 추가해줌.
		{
			tagBlackBoardData<CGameObject*>* pTarget = new tagBlackBoardData<CGameObject*>(pPlayer);

			hashBlackBoard.emplace(TEXT("Target"), pTarget);
		}

		return true;
	}
	else // 시야에 없다면
	{
		BLACKBOARD& hashBlackBoard = m_pBehaviorTree->GetBlackBoard();
		if (FAILED(m_pGameInstance->StopSound(CHANNELID::CHANNEL_ENEMY2)))
			__debugbreak();

		if (tTarget != hashBlackBoard.end())	// 근데 키값이 있다면 제거
		{
			hashBlackBoard.erase(tTarget);
		}

		return false;
	}
}

_bool CHellBrute_BT_Fire::IsZeroHP()
{
	if(static_cast<CMonsterController*>(m_pController)->IsZeroHP())
		return true;

	return false;
}

void CHellBrute_BT_Fire::Fire()
{
	CGameObject* pAmmo = nullptr;
	CStrife_Ammo::AMMOPROPS tProps;

	Vec3 vFireOffset;

	CTransform* pTransform = m_pGameObject->GetTransform();
	Quaternion qRot = m_pGameObject->GetTransform()->GetRotationQuaternion();

	tProps = { CStrife_Ammo::AmmoType::DEFAULT, 7, 0, 10, 7.5f * pTransform->GetForward(), false, 1.8f };
	pAmmo = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Strife_Ammo_Default"), LAYERTAG::UNIT_AIR, &tProps);
	vFireOffset = pTransform->GetPosition() + 3.f * pTransform->GetForward() + 1.9f * pTransform->GetRight() + 1.5f * pTransform->GetUp();

	CTransform* pAmmoTransform = pAmmo->GetTransform();
	pAmmoTransform->SetScale(Vec3(0.4f, 3.2f, 1.f));
	pAmmoTransform->Rotate(qRot);
	pAmmoTransform->RotateYAxisFixed(Vec3(0.f, 2.5f, 0.f));
	pAmmoTransform->SetPosition(vFireOffset);

	static_cast<CStrife_Ammo_Default*>(pAmmo)->SetColor(Color(1.f, 0.15f, 0.23f));
}

CHellBrute_BT_Fire* CHellBrute_BT_Fire::Create(CGameObject* pGameObject, CBehaviorTree* pBehaviorTree, const BEHAVEANIMS& tBehaveAnim, CMonoBehaviour* pController)
{
	CHellBrute_BT_Fire* pInstance = new CHellBrute_BT_Fire;

	if (FAILED(pInstance->Initialize(pGameObject, pBehaviorTree, tBehaveAnim, pController)))
	{
		MSG_BOX("Failed to Created : CHellBrute_BT_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute_BT_Fire::Free()
{
	Super::Free();
}