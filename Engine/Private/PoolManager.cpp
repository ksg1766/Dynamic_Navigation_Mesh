#include "PoolManager.h"
#include "ObjectManager.h"
#include "EventManager.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CPoolManager)

CPoolManager::CPoolManager()
{
}

HRESULT CPoolManager::Reserve_Pool(const wstring& strObjectName, const _uint& iReserveCount, void* pArg)
{
	CGameObject* pPrototype = CObjectManager::GetInstance()->Find_Prototype(strObjectName);
	if (nullptr == pPrototype)
		return E_FAIL;

	for (_uint i = 0; i < iReserveCount; ++i)
	{
		CGameObject* pGameObject = pPrototype->Clone(pArg);
		if (nullptr == pGameObject)
			return E_FAIL;

		POOLS::iterator iter = m_hashPools.find(strObjectName);
		iter->second.push(pGameObject);
	}
}

void CPoolManager::Spawn_Object(const wstring& strObjectName, const Vec3& vSpawnPos)
{
	POOLS::iterator iter = m_hashPools.find(strObjectName);

	if (iter == m_hashPools.end())
		return;

	CGameObject* pGameObject = iter->second.front();
	iter->second.pop();

	const LAYERTAG& eLayerTag = pGameObject->GetLayerTag();
	
	const map<LAYERTAG, CLayer*>& mapLayers = CObjectManager::GetInstance()->GetCurrentLevelLayers();

	const auto& _iter = mapLayers.find(LAYERTAG::WALL);
	if (mapLayers.end() != _iter)
		_iter->second->Add_GameObject(pGameObject, eLayerTag);
	else
		__debugbreak();
}

void CPoolManager::Restore_Object(CGameObject* pGameObject)
{
	POOLS::iterator iter = m_hashPools.find(pGameObject->GetObjectTag());
	iter->second.push(pGameObject);
}

void CPoolManager::Free()
{
	__super::Free();
}