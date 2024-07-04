#include "stdafx.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "P_Strife_State_Aim.h"
#include "PlayerController.h"

CP_Strife_State_Aim::CP_Strife_State_Aim()
	:Super()
{
	m_strStateName = TEXT("Aim");
	m_vecTransition.push_back(TEXT("Idle"));
	m_vecTransition.push_back(TEXT("Run"));
	m_vecTransition.push_back(TEXT("Jump"));
	m_vecTransition.push_back(TEXT("Dash"));
	m_vecTransition.push_back(TEXT("Impact"));
}

HRESULT CP_Strife_State_Aim::Enter(_int i)
{
	m_iCurrAnimation = i;
	Super::Enter(m_vecAnimIndexTime[i].first);

	m_fFR_Default_Timer = 0.1f;

	return S_OK;
}

void CP_Strife_State_Aim::Tick(const _float& fTimeDelta)
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	Input(fTimeDelta);
	pController->ForceHeight();
}

const wstring& CP_Strife_State_Aim::LateTick(const _float& fTimeDelta)
{
	return Transition();
}

void CP_Strife_State_Aim::Exit()
{
}

const wstring& CP_Strife_State_Aim::Transition()
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);
	
	if (pController->IsDash())
		return m_vecTransition[Trans::DASH];

	if (pController->IsJump())
		return m_vecTransition[Trans::JUMP];

	if (!pController->IsAim())
	{
		return m_vecTransition[Trans::IDLE];
	}

	if (Anims::AIM_IDLE == m_iCurrAnimation)
	{
		if (pController->IsRun())
		{
			Enter(Anims::AIM_WALK);
		}
	}
	else if (Anims::AIM_WALK == m_iCurrAnimation)
	{
		if (!pController->IsRun())
		{
			Enter(Anims::AIM_IDLE);
		}
	}
	
	return m_strStateName;
}

void CP_Strife_State_Aim::Input(const _float& fTimeDelta)
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W))
	{
		pController->GetTranslateMessage(Vec3::UnitZ);
	}
	if (KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A))
	{
		pController->GetTranslateMessage(-Vec3::UnitX);
	}
	if (KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S))
	{
		pController->GetTranslateMessage(-Vec3::UnitZ);
	}
	if (KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D))
	{
		pController->GetTranslateMessage(Vec3::UnitX);
	}

	const POINT& p = CGameInstance::GetInstance()->GetMousePos();
	if (p.x > 1440 || p.x < 0 || p.y > 810 || p.y < 0)
		return;

	if (MOUSE_DOWN(MOUSEKEYSTATE::DIM_LB) || MOUSE_PRESSING(MOUSEKEYSTATE::DIM_LB))
	{
		const POINT& p = m_pGameInstance->GetMousePos();
		Vec3 vPickPos;	_float fDistance;

		pController->Pick(p.x, p.y, vPickPos, fDistance);
		pController->Look(vPickPos, fTimeDelta);

		m_fFR_Default_Timer -= fTimeDelta;
		if (m_fFR_Default_Timer < 0.f)
		{
			pController->GetFireMessage(CStrife_Ammo::AmmoType::DEFAULT);
			m_fFR_Default_Timer = m_fFR_Default;
		}
	}
	//if (MOUSE_DOWN(MOUSEKEYSTATE::DIM_RB) || MOUSE_PRESSING(MOUSEKEYSTATE::DIM_RB))
	//{
	//	const POINT& p = m_pGameInstance->GetMousePos();
	//	Vec3 vPickPos;	_float fDistance;

	//	pController->Pick(p.x, p.y, vPickPos, fDistance);
	//	pController->Look(vPickPos, fTimeDelta);

	//	pController->GetFireMessage(CStrife_Ammo::AmmoType::BEAM);
	//}
	if (MOUSE_DOWN(MOUSEKEYSTATE::DIM_RB) || MOUSE_PRESSING(MOUSEKEYSTATE::DIM_RB))
	{
		const POINT& p = m_pGameInstance->GetMousePos();
		Vec3 vPickPos;	_float fDistance;

		pController->Pick(p.x, p.y, vPickPos, fDistance);
		pController->Look(vPickPos, fTimeDelta);

		m_fFR_Default_Timer -= fTimeDelta;
		if (m_fFR_Default_Timer < 0.f)
		{
			pController->GetFireMessage(CStrife_Ammo::AmmoType::NATURE);
			m_fFR_Default_Timer = 4.f * m_fFR_Default;
		}
	}
}

CP_Strife_State_Aim* CP_Strife_State_Aim::Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	CP_Strife_State_Aim* pInstance = new CP_Strife_State_Aim();

	if (FAILED(pInstance->Initialize(pGameObject, tStateAnim, pController)))
	{
		MSG_BOX("Failed to Created : CP_Strife_State_Aim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife_State_Aim::Free()
{
	Super::Free();
}