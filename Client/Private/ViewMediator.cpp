#include "stdafx.h"
#include "ViewMediator.h"
#include "AnimationView.h"
#include "PrefabsView.h"
#include "LayersView.h"
#include "TransformView.h"
#include "SaveLoadView.h"
#include "NavMeshView.h"

CViewMediator::CViewMediator()
{
}

CViewMediator::~CViewMediator()
{
}

void CViewMediator::OnNotifiedSelected(CGameObject* pGameObject)
{
	m_pTransformView->SetObject(pGameObject);
}

void CViewMediator::OnNotifiedPickingOn(CView* pSender)
{
	if(pSender == m_pPrefabsView)
	{
		m_pLayersView->DeactivatePicking();
		m_pNavMeshView->DeactivatePicking();
	}
	else if (pSender == m_pLayersView)
	{
		m_pPrefabsView->DeactivatePicking();
		m_pNavMeshView->DeactivatePicking();
	}
	else if (pSender == m_pNavMeshView)
	{
		m_pLayersView->DeactivatePicking();
		m_pPrefabsView->DeactivatePicking();
	}
}

void CViewMediator::SetPrefabsView(CPrefabsView* pPrefabsView)
{ 
	m_pPrefabsView = pPrefabsView;
	Safe_AddRef(m_pPrefabsView);
	m_pPrefabsView->SetMediator(this);
}

void CViewMediator::SetLayersView(CLayersView* pLayersView)
{ 
	m_pLayersView = pLayersView;
	Safe_AddRef(m_pLayersView);
	m_pLayersView->SetMediator(this);
}

void CViewMediator::SetTransformView(CTransformView* pTransformView)
{ 
	m_pTransformView = pTransformView;
	Safe_AddRef(m_pTransformView);
	m_pTransformView->SetMediator(this);
}

void CViewMediator::SetSaveLoadView(CSaveLoadView* pSaveLoadView)
{ 
	m_pSaveLoadView = pSaveLoadView;
	Safe_AddRef(m_pSaveLoadView);
	m_pSaveLoadView->SetMediator(this);
}

void CViewMediator::SetAnimationView(CAnimationView* pAnimationView)
{
	m_pAnimationView = pAnimationView;
	Safe_AddRef(m_pAnimationView);
	m_pAnimationView->SetMediator(this);
}

void CViewMediator::SetNavMeshView(CNavMeshView* pNavMeshView)
{
	m_pNavMeshView = pNavMeshView;
	Safe_AddRef(m_pNavMeshView);
	m_pNavMeshView->SetMediator(this);
}

void CViewMediator::Free()
{
	Safe_Release(m_pPrefabsView);
	Safe_Release(m_pLayersView);
	Safe_Release(m_pTransformView);
	Safe_Release(m_pSaveLoadView);
	Safe_Release(m_pAnimationView);
	Safe_Release(m_pNavMeshView);
}
