#include "stdafx.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "P_Strife_State_Impact.h"

CP_Strife_State_Impact::CP_Strife_State_Impact()
	:Super()
{
	m_strStateName = TEXT("Impact");
	m_vecTransition.push_back(TEXT("Idle"));
	m_vecTransition.push_back(TEXT("Run"));
	m_vecTransition.push_back(TEXT("Aim"));
	m_vecTransition.push_back(TEXT("Jump"));
	m_vecTransition.push_back(TEXT("Dash"));
}

HRESULT CP_Strife_State_Impact::Enter(_int i)
{
	m_iCurrAnimation = i;
	Super::Enter(m_vecAnimIndexTime[i].first);

	return S_OK;
}

void CP_Strife_State_Impact::Tick(const _float& fTimeDelta)
{
}

const wstring& CP_Strife_State_Impact::LateTick(const _float& fTimeDelta)
{
	return Transition();
}

void CP_Strife_State_Impact::Exit()
{
}

const wstring& CP_Strife_State_Impact::Transition()
{
	//if (m_pController->Run())
	//	return m_vecTransition[0];

	return m_strStateName;
}

CP_Strife_State_Impact* CP_Strife_State_Impact::Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	CP_Strife_State_Impact* pInstance = new CP_Strife_State_Impact();

	if (FAILED(pInstance->Initialize(pGameObject, tStateAnim, pController)))
	{
		MSG_BOX("Failed to Created : CP_Strife_State_Impact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife_State_Impact::Free()
{
	Super::Free();
}