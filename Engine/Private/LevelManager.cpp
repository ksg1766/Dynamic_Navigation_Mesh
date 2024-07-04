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

		/* ������ �߰��Ǿ��ִ� ������ �ڿ��� �����Ѵ�. */
		pGameInstance->Clear(m_iPreviousLevelIndex);

		Safe_Release(pGameInstance);
	}

	/* �������� ��ü�� �Ăt����. */
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	//m_iCurrentLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevelManager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
