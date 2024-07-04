#include "stdafx.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "P_Strife_State_Jump.h"
#include "PlayerController.h"

CP_Strife_State_Jump::CP_Strife_State_Jump()
	:Super()
{
	m_strStateName = TEXT("Jump");
	m_vecTransition.push_back(TEXT("Idle"));
	m_vecTransition.push_back(TEXT("Run"));
	m_vecTransition.push_back(TEXT("Aim"));
	m_vecTransition.push_back(TEXT("Dash"));
	m_vecTransition.push_back(TEXT("Impact"));
}

HRESULT CP_Strife_State_Jump::Enter(_int i)
{
	m_iCurrAnimation = i;
	Super::Enter(m_vecAnimIndexTime[i].first);
	
	if (i == Anims::JUMP || i == Anims::JUMP_DOUBLE)
	{
		static_cast<CPlayerController*>(m_pController)->GetJumpMessage(true);
		i == Anims::JUMP ? m_pGameInstance->PlaySoundFile(TEXT("char_strife_jump_01.ogg"), CHANNELID::CHANNEL_MOVE, 0.3f)
			: m_pGameInstance->PlaySoundFile(TEXT("char_strife_jumpdouble_01.ogg"), CHANNELID::CHANNEL_MOVE, 0.3f);
	}
	else if (i == Anims::LAND || i == Anims::JUMP_LAND_HEAVY)
	{
		static_cast<CPlayerController*>(m_pController)->GetJumpMessage(false);
		m_pGameInstance->PlaySoundFile(TEXT("char_strife_land_heavy.ogg"), CHANNELID::CHANNEL_MOVE, 0.3f);
	}

	return S_OK;
}

void CP_Strife_State_Jump::Tick(const _float& fTimeDelta)
{
	m_fTimeSum += fTimeDelta;
	Input(fTimeDelta);
}

const wstring& CP_Strife_State_Jump::LateTick(const _float& fTimeDelta)
{
	return Transition();
}

void CP_Strife_State_Jump::Exit()
{
}

const wstring& CP_Strife_State_Jump::Transition()
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	/*if (pController->Aim())
		return m_vecTransition[Trans::AIM];*/

	if (Anims::JUMP == m_iCurrAnimation)
	{
		if (pController->GetHeightOffset() > 0.0f)
		{
			pController->ForceHeight();
			Enter(Anims::LAND);
			return m_strStateName;
		}

		if (m_fTimeSum > m_vecAnimIndexTime[m_iCurrAnimation].second)
		{
			Enter(Anims::FALL);
			return m_strStateName;
		}

		if (pController->IsJump())
		{
			m_bDoubleJump = true;
			Enter(Anims::JUMP_DOUBLE);
			return m_strStateName;
		}
	}
	else if (Anims::JUMP_DOUBLE == m_iCurrAnimation)
	{
		if (pController->GetHeightOffset() > 0.0f)
		{
			pController->ForceHeight();
			Enter(Anims::JUMP_LAND_HEAVY);
			return m_strStateName;
		}

		if (m_fTimeSum > m_vecAnimIndexTime[m_iCurrAnimation].second)
		{
			Enter(Anims::FALL);
			return m_strStateName;
		}
	}
	else if (Anims::FALL == m_iCurrAnimation)
	{
		if (pController->GetHeightOffset() > 0.0f)
		{
			pController->ForceHeight();
			m_bDoubleJump = false;
			Enter(Anims::JUMP_LAND_HEAVY);
			return m_strStateName;
		}

		if (!m_bDoubleJump && pController->IsJump())
		{
			m_bDoubleJump = true;
			Enter(Anims::JUMP_DOUBLE);
			return m_strStateName;
		}
	}
	else if (Anims::LAND == m_iCurrAnimation)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[m_iCurrAnimation].second * 0.4f)
		{
			return m_vecTransition[Trans::IDLE];
		}
	}
	else if (Anims::JUMP_LAND_HEAVY == m_iCurrAnimation)
	{
		if (m_fTimeSum > m_vecAnimIndexTime[m_iCurrAnimation].second * 0.4f)
		{
			return m_vecTransition[Trans::IDLE];
		}
	}

	return m_strStateName;
}

void CP_Strife_State_Jump::Input(const _float& fTimeDelta)
{
	CPlayerController* pController = static_cast<CPlayerController*>(m_pController);

	if (Anims::LAND == m_iCurrAnimation || Anims::JUMP_LAND_HEAVY == m_iCurrAnimation) return;
	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W)) pController->GetMoveMessage(Vec3::UnitZ);
	if (KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A)) pController->GetMoveMessage(-Vec3::UnitX);
	if (KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S)) pController->GetMoveMessage(-Vec3::UnitZ);
	if (KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D)) pController->GetMoveMessage(Vec3::UnitX);
}

CP_Strife_State_Jump* CP_Strife_State_Jump::Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	CP_Strife_State_Jump* pInstance = new CP_Strife_State_Jump();

	if (FAILED(pInstance->Initialize(pGameObject, tStateAnim, pController)))
	{
		MSG_BOX("Failed to Created : CP_Strife_State_Jump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife_State_Jump::Free()
{
	Super::Free();
}