#include "stdafx.h"
#include "ViewMediator.h"
#include "PrefabsView.h"
#include "NavMeshView.h"

CViewMediator::CViewMediator()
{
}

CViewMediator::~CViewMediator()
{
}

void CViewMediator::OnNotifiedPickingOn(CView* pSender)
{
	if(pSender == m_pPrefabsView)
	{
		m_pNavMeshView->DeactivatePicking();
	}
	else if (pSender == m_pNavMeshView)
	{
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

void CViewMediator::SetNavMeshView(CNavMeshView* pNavMeshView)
{
	m_pNavMeshView = pNavMeshView;
	Safe_AddRef(m_pNavMeshView);
	m_pNavMeshView->SetMediator(this);
}

void CViewMediator::Free()
{
	Safe_Release(m_pPrefabsView);
	Safe_Release(m_pNavMeshView);
}
