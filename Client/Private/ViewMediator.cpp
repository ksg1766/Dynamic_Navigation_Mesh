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

void CViewMediator::OnNotifiedPlaceObject(const wstring& strObjectTag, const Matrix& matWorld, OUT CGameObject*& pGameObject)
{
	m_pPrefabsView->PlaceObstacle(strObjectTag, matWorld, pGameObject);
}

void CViewMediator::OnNotifiedPlaceObstacle(CGameObject* const pGameObject)
{
	m_pNavMeshView->DynamicCreate(pGameObject);
}

void CViewMediator::OnNotifiedPlaceObstacle(const wstring& strObjectTag, const Vec3& vPickPos)
{
	m_pNavMeshView->DynamicCreate(strObjectTag, vPickPos);
}

void CViewMediator::OnNotifiedTransformChanged(CGameObject* const pGameObject)
{
	m_pNavMeshView->UpdateObstacleTransform(pGameObject);
}

void CViewMediator::OnNotifiedTerrainChanged(CTerrain* const pTerrainBuffer)
{
	m_pPrefabsView->ChangeTerrain(pTerrainBuffer);
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
