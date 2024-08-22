#include "EventManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CEventManager);

CEventManager::CEventManager()
{
}

void CEventManager::FinalTick()
{
	for (size_t i = 0; i < m_vecDead.size(); ++i)
	{
		// 여기에서 삭제 해주고, Level에서는 벡터 원소만 날려 줌.
		Safe_Release(m_vecDead[i]);
	}
	m_vecDead.clear();

	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		Execute(m_vecEvent[i]);
	}
	m_vecEvent.clear();
}

void CEventManager::CreateObject(CGameObject*& pObj, const LAYERTAG& eLayer)
{
	tagEvent evn = {};
	evn.eEvent = EVENT_TYPE::CREATE_OBJECT;
	evn.lParam = (DWORD_PTR)pObj;
	evn.wParam = (DWORD_PTR)eLayer;

	AddEvent(evn);
}

CGameObject* CEventManager::CreateObject(const wstring& strPrototypeTag, const LAYERTAG& eLayer, void* pArg)
{
	CObjectManager* pObjectManager = GET_INSTANCE(CObjectManager);
	CLevelManager* pLevelManager = GET_INSTANCE(CLevelManager);

	CGameObject* pPrototype = pObjectManager->Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
	{
		RELEASE_INSTANCE(CObjectManager);
		RELEASE_INSTANCE(CLevelManager);
		return nullptr;
	}

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		RELEASE_INSTANCE(CObjectManager);
		RELEASE_INSTANCE(CLevelManager);
		return nullptr;
	}
	_uint iCurrentLevelIndex = pLevelManager->GetCurrentLevelIndex();
	CLayer* pLayer = pObjectManager->Find_Layer(iCurrentLevelIndex, eLayer);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->SetLayerTag(eLayer);

		pObjectManager->GetCurrentLevelLayers().emplace(eLayer, pLayer);

		CreateObject(pGameObject, eLayer);
	}
	else
		CreateObject(pGameObject, eLayer);
	
	RELEASE_INSTANCE(CObjectManager);
	RELEASE_INSTANCE(CLevelManager);

	return pGameObject;
}

void CEventManager::DeleteObject(CGameObject*& pObj)
{
	tagEvent evn = {};
	evn.eEvent = EVENT_TYPE::DELETE_OBJECT;
	evn.lParam = (DWORD_PTR)pObj;

	AddEvent(evn);
}

void CEventManager::LevelChange(CLevel*& pLevel, _uint& iLevelId)
{
	tagEvent evn = {};
	evn.eEvent = EVENT_TYPE::LEVEL_CHANGE;
	evn.lParam = (DWORD_PTR)pLevel;
	evn.wParam = (DWORD_PTR)iLevelId;

	AddEvent(evn);
}

void CEventManager::Execute(const tagEvent& eve)
{
	switch (eve.eEvent)
	{
	case EVENT_TYPE::CREATE_OBJECT:
	{
		CGameObject* pNewObject = (CGameObject*)eve.lParam;
		LAYERTAG eLayer = (LAYERTAG)eve.wParam;

		map<LAYERTAG, CLayer*>& mapLayers = CObjectManager::GetInstance()->GetCurrentLevelLayers();
		auto iter = mapLayers.find(eLayer);

		if (mapLayers.end() != iter)
		{
			//(*CObjectManager::GetInstance()->GetCurrentLevelLayers())[eLayer]->Add_GameObject(pNewObject, eLayer);
			iter->second->Add_GameObject(pNewObject, eLayer);
		}
	}
	break;
	case EVENT_TYPE::DELETE_OBJECT:
	{
		CGameObject* pDeadObject = (CGameObject*)eve.lParam;

		pDeadObject->SetDeadState(true);
		m_vecDead.push_back(pDeadObject);
	}
	break;
	case EVENT_TYPE::LEVEL_CHANGE:
	{
		
	}
	break;
	}
}

void CEventManager::Free()
{
	Super::Free();
}
