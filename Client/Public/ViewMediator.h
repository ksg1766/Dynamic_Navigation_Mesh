#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)

class CGameInstance;
class CGameObject;

END

BEGIN(Client)

class CView;
class CPrefabsView;
class CLayersView;
class CTransformView;
class CSaveLoadView;
class CAnimationView;
class CNavMeshView;
class CViewMediator : public CBase
{
	using Super = CBase;

public:
	CViewMediator();
	virtual ~CViewMediator();

public:
	void	OnNotifiedSelected(CGameObject* pGameObject);
	void	OnNotifiedPickingOn(CView* pSender);

	void	SetPrefabsView(CPrefabsView* pPrefabsView);
	void	SetLayersView(CLayersView* pLayersView);
	void	SetTransformView(CTransformView* pTransformView);
	void	SetSaveLoadView(CSaveLoadView* pSaveLoadView);
	void	SetAnimationView(CAnimationView* pAnimationView);
	void	SetNavMeshView(CNavMeshView* pNavMeshView);

protected:
	CGameInstance*	m_pGameInstance = nullptr;

	_bool			m_IsPickingActivated = false;

	CPrefabsView*	m_pPrefabsView = nullptr;
	CLayersView*	m_pLayersView = nullptr;
	CTransformView*	m_pTransformView = nullptr;
	CSaveLoadView*	m_pSaveLoadView = nullptr;
	CAnimationView*	m_pAnimationView = nullptr;
	CNavMeshView*	m_pNavMeshView = nullptr;

public:
	virtual void Free() override;
};

END