#include "..\Public\ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"
#include "CameraManager.h"

IMPLEMENT_SINGLETON(CObjectManager)

CObjectManager::CObjectManager()
{
}

HRESULT CObjectManager::Reserve_Manager(_uint iNumLevels)
{
	if (m_vecLayers.size())
		return E_FAIL;

	m_vecLayers.resize(iNumLevels);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObjectManager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	pPrototype->SetObjectTag(strPrototypeTag.substr(21));

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CGameObject* CObjectManager::Add_GameObject(_uint iLevelIndex, const LAYERTAG& eLayerTag, const wstring& strPrototypeTag, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	if (m_vecLayers[iLevelIndex].size() < (_uint)LAYERTAG::LAYER_END)
	{
		CLayer* pLayer = CLayer::Create();

		for (_uint i = 0; i < (_uint)LAYERTAG::LAYER_END; ++i)
		{
			LAYERTAG e = (LAYERTAG)i;

			pLayer->SetLayerTag(e);

			m_vecLayers[iLevelIndex].emplace(eLayerTag, pLayer);
		}
	}

	CLayer*		pLayer = Find_Layer(iLevelIndex, eLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->SetLayerTag(eLayerTag);

		m_vecLayers[iLevelIndex].emplace(eLayerTag, pLayer);
		
		pLayer->Add_GameObject(pGameObject, eLayerTag);
	}
	else
		pLayer->Add_GameObject(pGameObject, eLayerTag);

	return pGameObject;
}

void CObjectManager::Tick(const _float& fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_vecLayers[i])
		{
			if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END
				|| (_uint)Pair.first == (_uint)LAYERTAG::DYNAMIC_LAYER_END)
				continue;

			if ((_uint)Pair.first == (_uint)LAYERTAG::CAMERA)
			{
				CCameraManager::GetInstance()->Tick(fTimeDelta);
				continue;
			}

			if ((_uint)Pair.first < (_uint)LAYERTAG::STATIC_LAYER_END)
				Pair.second->Tick(fTimeDelta);
			// Temp
			if (Pair.first == LAYERTAG::TERRAIN)
				Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObjectManager::LateTick(const _float& fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_vecLayers[i])
		{
			if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END
				|| (_uint)Pair.first == (_uint)LAYERTAG::DYNAMIC_LAYER_END)
				continue;

			if ((_uint)Pair.first == (_uint)LAYERTAG::CAMERA)
			{
				CCameraManager::GetInstance()->LateTick(fTimeDelta);
				continue;
			}

			if ((_uint)Pair.first < (_uint)LAYERTAG::STATIC_LAYER_END)
				Pair.second->LateTick(fTimeDelta);
			// Temp
			if (Pair.first == LAYERTAG::TERRAIN)
				Pair.second->LateTick(fTimeDelta);
		}
	}
}

void CObjectManager::DebugRender()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_vecLayers[i])
		{
			if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END
				|| (_uint)Pair.first == (_uint)LAYERTAG::DYNAMIC_LAYER_END)
				continue;

			if ((_uint)Pair.first < (_uint)LAYERTAG::STATIC_LAYER_END)
				Pair.second->DebugRender();
			// Temp
			if (Pair.first == LAYERTAG::TERRAIN)
				Pair.second->DebugRender();
		}
	}
}

void CObjectManager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_vecLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_vecLayers[iLevelIndex].clear();
}

CGameObject * CObjectManager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObjectManager::Find_Layer(_uint iLevelIndex, const LAYERTAG& eLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_vecLayers[iLevelIndex].find(eLayerTag);

	if (iter == m_vecLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObjectManager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_vecLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_vecLayers[i].clear();
	}

	m_vecLayers.clear();

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
