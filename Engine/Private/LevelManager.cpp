#include "..\Public\LevelManager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevelManager)

CLevelManager::CLevelManager()
{
}

void CLevelManager::Tick(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevelManager::LateTick(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->LateTick(fTimeDelta);
}

void CLevelManager::DebugRender()
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->DebugRender();
}

HRESULT CLevelManager::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance*	pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		/* 기존에 추가되어있던 레벨용 자원을 정리한다. */
		pGameInstance->Clear(m_iPreviousLevelIndex);

		Safe_Release(pGameInstance);
	}

	/* 기존레벨 객체를 파굏란다. */
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	//m_iCurrentLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevelManager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
