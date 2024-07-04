#include "..\Public\ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"
#include "RigidBodyBase.h"
#include "CollisionManager.h"
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
	/* 복제할 사본을 찾는다. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	/* 원형을 복제하여 사본을 만든다. */
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

			//m_pLayers[iLevelIndex].emplace(eLayerTag, pLayer);
			m_vecLayers[iLevelIndex].emplace(eLayerTag, pLayer);
		}
	}

	CLayer*		pLayer = Find_Layer(iLevelIndex, eLayerTag);

	/* 내가 추가할려고 하는 레이어가 없었다. */
	if (nullptr == pLayer)
	{
		/* 레이어를 만들자. */
		pLayer = CLayer::Create();
		pLayer->SetLayerTag(eLayerTag);

		/* 생성한 레이어를 iLevelIndex용 레이어로 추가하였다. */
		m_vecLayers[iLevelIndex].emplace(eLayerTag, pLayer);
		
		/* 레이어에 객체를 추가하자. */
		pLayer->Add_GameObject(pGameObject, eLayerTag);
		//
	}
	else /* 추가하고자 하는 레이어가 있었다. */
		pLayer->Add_GameObject(pGameObject, eLayerTag);

	return pGameObject;
}

void CObjectManager::Tick(const _float& fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		if (i == 2)		// GAMEPLAY
		{
			for (auto& Pair : m_vecLayers[i])
			{
				if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END)
					continue;
				
				if ((_uint)Pair.first == (_uint)LAYERTAG::CAMERA)
				{
					CCameraManager::GetInstance()->Tick(fTimeDelta);
					continue;
				}

				if ((_uint)Pair.first < (_uint)LAYERTAG::DYNAMIC_LAYER_END)
					Pair.second->Tick(fTimeDelta);
				// Temp
				if (Pair.first == LAYERTAG::TERRAIN)
					Pair.second->Tick(fTimeDelta);
			}
		}
		else if (i != 2)		// !GAMEPLAY
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
}

void CObjectManager::LateTick(const _float& fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		if (i == 2)		// GAMEPLAY
		{
			for (auto& Pair : m_vecLayers[i])
			{
				if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END)
					continue;

				if ((_uint)Pair.first == (_uint)LAYERTAG::CAMERA)
				{
					CCameraManager::GetInstance()->LateTick(fTimeDelta);
					continue;
				}

				if ((_uint)Pair.first < (_uint)LAYERTAG::DYNAMIC_LAYER_END)
					Pair.second->LateTick(fTimeDelta);
				// Temp
				if (Pair.first == LAYERTAG::TERRAIN)
					Pair.second->LateTick(fTimeDelta);
			}
		}
		else if (i != 2)	// GAMEPLAY
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
}

void CObjectManager::DebugRender()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		if (i == 2)		// GAMEPLAY
		{
			//for (auto& Pair : m_pLayers[i])
			for (auto& Pair : m_vecLayers[i])
			{
				if ((_uint)Pair.first == (_uint)LAYERTAG::DEFAULT_LAYER_END)
					continue;

				if ((_uint)Pair.first < (_uint)LAYERTAG::DYNAMIC_LAYER_END)
					Pair.second->DebugRender();
				// Temp
				if (Pair.first == LAYERTAG::TERRAIN)
					Pair.second->DebugRender();
			}
		}
		else if (i != 2)	// !GAMEPLAY
		{
			//for (auto& Pair : m_pLayers[i])
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
}

void CObjectManager::Clear(_uint iLevelIndex)
{
	//for (auto& Pair : m_pLayers[iLevelIndex])
	for (auto& Pair : m_vecLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	//m_pLayers[iLevelIndex].clear();
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

	//auto	iter = m_pLayers[iLevelIndex].find(eLayerTag);
	auto	iter = m_vecLayers[iLevelIndex].find(eLayerTag);

	//if (iter == m_pLayers[iLevelIndex].end())
	if (iter == m_vecLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObjectManager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		//for (auto& Pair : m_pLayers[i])
		for (auto& Pair : m_vecLayers[i])
		{
			Safe_Release(Pair.second);
		}
		//m_pLayers[i].clear();
		m_vecLayers[i].clear();
	}

	//Safe_Delete_Array(m_pLayers);
	m_vecLayers.clear();

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
