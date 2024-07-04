#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameObject;

enum class EVENT_TYPE { CREATE_OBJECT, DELETE_OBJECT, LEVEL_CHANGE, EVENT_END };

struct tagEvent
{
	EVENT_TYPE	eEvent;
	DWORD_PTR	lParam;
	DWORD_PTR	wParam;
};

class CEventManager final : public CBase
{
	DECLARE_SINGLETON(CEventManager);
	using Super = CBase;
private:
	CEventManager();
	virtual ~CEventManager() = default;

public:
	void FinalTick();

	CGameObject* CreateObject(const wstring& strPrototypeTag, const LAYERTAG& eLayer, void* pArg);

	void DeleteObject(CGameObject*& pObj);
	void LevelChange(class CLevel*& pLevel, _uint& iLevelId);

private:
	void AddEvent(const tagEvent& eve) { m_vecEvent.push_back(eve); }
	void Execute(const tagEvent& eve);

	void CreateObject(CGameObject*& pObj, const LAYERTAG& eLayer);

private:
	vector<tagEvent> m_vecEvent;
	vector<CGameObject*> m_vecCreate;
	vector<CGameObject*> m_vecDead;

public:
	virtual void Free() override;
};

END