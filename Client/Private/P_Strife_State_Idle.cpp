#include "stdafx.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "P_Strife_State_Idle.h"
#include "PlayerController.h"

CP_Strife_State_Idle::CP_Strife_State_Idle()
	:Super()
{
	m_strStateName = TEXT("Idle");
	m_vecTransition.push_back(TEXT("Run"));
	m_vecTransition.push_back(TEXT("Aim"));
	m_vecTransition.push_back(TEXT("Jump"));
	m_vecTransition.push_back(TEXT("Dash"));
	m_vecTransition.push_back(TEXT("Impact"));
}

HRESULT CP_Strife_State_Idle::Enter(_int i)
{
	m_iCurrAnimation = i;
	Super::Enter(m_vecAnimIndexTime[i].first);

	return S_OK;
}

void CP_Strife_State_Idle::Tick(const _float& fTimeDelta)
{
}

const wstring& CP_Strife_State_Idle::LateTick(const _float& fTimeDelta)
{
	return Transition();
}

void CP_Strife_State_Idle::Exit()
{
}

const wstring& CP_Strife_State_Idle::Transition()
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);
	
	if (pController->IsRun())
		return m_vecTransition[Trans::RUN];

	if (pController->IsJump())
		return m_vecTransition[Trans::JUMP];

	if (pController->IsDash())
		return m_vecTransition[Trans::DASH];

	if (pController->IsAim())
		return m_vecTransition[Trans::AIM];

	/*if ()
		return m_vecTransition[Trans::IMPACT];*/

	return m_strStateName;
}

CP_Strife_State_Idle* CP_Strife_State_Idle::Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	CP_Strife_State_Idle* pInstance = new CP_Strife_State_Idle();

	if (FAILED(pInstance->Initialize(pGameObject, tStateAnim, pController)))
	{
		MSG_BOX("Failed to Created : CP_Strife_State_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife_State_Idle::Free()
{
	Super::Free();
}