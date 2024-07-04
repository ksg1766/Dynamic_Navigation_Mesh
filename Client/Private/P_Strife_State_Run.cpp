#include "stdafx.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "P_Strife_State_Run.h"
#include "PlayerController.h"

constexpr auto EPSILON = 0.03f;

CP_Strife_State_Run::CP_Strife_State_Run()
	:Super()
{
	m_strStateName = TEXT("Run");
	m_vecTransition.push_back(TEXT("Idle"));
	m_vecTransition.push_back(TEXT("Aim"));
	m_vecTransition.push_back(TEXT("Jump"));
	m_vecTransition.push_back(TEXT("Dash"));
	m_vecTransition.push_back(TEXT("Impact"));
}

HRESULT CP_Strife_State_Run::Enter(_int i)
{	
	m_iCurrAnimation = i;
	Super::Enter(m_vecAnimIndexTime[i].first);

	return S_OK;
}

void CP_Strife_State_Run::Tick(const _float& fTimeDelta)
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	Input(fTimeDelta);
	pController->ForceHeight();

	m_fDefault_Timer -= fTimeDelta;
	if (m_fDefault_Timer < 0.f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("char_strife_foot_01.ogg"), CHANNELID::CHANNEL_MOVE, 0.2f)))
			__debugbreak();
		m_fDefault_Timer = m_fDefault;
	}
}

const wstring& CP_Strife_State_Run::LateTick(const _float& fTimeDelta)
{
	return Transition();
}

void CP_Strife_State_Run::Exit()
{
}

const wstring& CP_Strife_State_Run::Transition()
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	if (pController->IsJump())
		return m_vecTransition[Trans::JUMP];

	if (pController->IsDash())
		return m_vecTransition[Trans::DASH];

	if (pController->IsAim())
		return m_vecTransition[Trans::AIM];

	if (!pController->IsRun())
		return m_vecTransition[Trans::IDLE];

	return m_strStateName;
}

void CP_Strife_State_Run::Input(const _float& fTimeDelta)
{	// 잘 고쳐보자
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W)) pController->GetMoveMessage(Vec3::UnitZ);
	if (KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A)) pController->GetMoveMessage(-Vec3::UnitX);
	if (KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S)) pController->GetMoveMessage(-Vec3::UnitZ);
	if (KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D)) pController->GetMoveMessage(Vec3::UnitX);
}

CP_Strife_State_Run* CP_Strife_State_Run::Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	CP_Strife_State_Run* pInstance = new CP_Strife_State_Run();

	if (FAILED(pInstance->Initialize(pGameObject, tStateAnim, pController)))
	{
		MSG_BOX("Failed to Created : CP_Strife_State_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife_State_Run::Free()
{
	Super::Free();
}

