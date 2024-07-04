#include "..\Public\ComponentManager.h"

IMPLEMENT_SINGLETON(CComponentManager)

CComponentManager::CComponentManager()
{

}

HRESULT CComponentManager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];
	
	return S_OK;
}

HRESULT CComponentManager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototypes || 
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponentManager::Clone_Component(CGameObject* pGameObject, _uint iLevelIndex, const wstring& strPrototypeTag, void * pArg)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*		pComponent = pPrototype->Clone(pGameObject, pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CComponent * CComponentManager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

void CComponentManager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
		{
			Safe_Release(Pair.second);
		}

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
