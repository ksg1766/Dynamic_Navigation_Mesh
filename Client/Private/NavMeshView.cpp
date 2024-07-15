#include "stdafx.h"
#include "NavMeshView.h"
#include "ViewMediator.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "FileUtils.h"
#include "Utils.h"
#include "Layer.h"
#include "DissolveManager.h"
#include "DebugDraw.h"
#include "Client_Macro.h"

struct CNavMeshView::CellData
{
	array<Vec3, POINT_END> vPoints = { Vec3::Zero, Vec3::Zero, Vec3::Zero };
	array<CellData*, LINE_END> arrNeighbors = { nullptr, nullptr, nullptr };

	// cache
	_bool	isDead = false;
	_bool	isNew = false;
	_bool	isOverSlope = false;
	_int	iSlope = -1;

	void CW()
	{
		Vec3 vA(vPoints[POINT_A].x, 0.f, vPoints[POINT_A].z);
		Vec3 vB(vPoints[POINT_B].x, 0.f, vPoints[POINT_B].z);
		Vec3 vC(vPoints[POINT_C].x, 0.f, vPoints[POINT_C].z);

		Vec3 vFlatAB = vB - vA;
		Vec3 vFlatBC = vC - vB;
		Vec3 vResult;
		vFlatAB.Cross(vFlatBC, vResult);

		if (vResult.y < 0.f)
		{
			::swap(vPoints[POINT_B], vPoints[POINT_C]);
		}
	}

	_bool ComparePoints(const Vec3& pSour, const Vec3& pDest)
	{
		if (pSour == vPoints[POINT_A])
		{
			if (pDest == vPoints[POINT_B])
				return true;

			if (pDest == vPoints[POINT_C])
				return true;
		}

		if (pSour == vPoints[POINT_B])
		{
			if (pDest == vPoints[POINT_A])
				return true;

			if (pDest == vPoints[POINT_C])
				return true;
		}

		if (pSour == vPoints[POINT_C])
		{
			if (pDest == vPoints[POINT_A])
				return true;

			if (pDest == vPoints[POINT_B])
				return true;
		}

		return false;
	}
};

CNavMeshView::CNavMeshView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CNavMeshView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

	m_pTerrainBuffer = reinterpret_cast<CTerrain*>(pArg);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t		iLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
		return E_FAIL;
	}

	m_vecPoints.push_back(Vec3(-512.0f, 0.f, -512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(-512.0f, 0.f, -512.0f), 2.f));

	m_vecPoints.push_back(Vec3(+512.0f, 0.f, -512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(+512.0f, 0.f, -512.0f), 2.f));
	
	m_vecPoints.push_back(Vec3(+512.0f, 0.f, +512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(+512.0f, 0.f, +512.0f), 2.f));

	m_vecPoints.push_back(Vec3(-512.0f, 0.f, +512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(-512.0f, 0.f, +512.0f), 2.f));

	m_iPointCount = m_vecPoints.size();

	UpdatePointList();
	//UpdateSegmentList();
	m_tIn.numberofsegments = m_iPointCount;
	if (0 < m_tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, m_tIn.segmentlist, m_tIn.numberofsegments * 2)

		// Points
		for (_int i = 0; i < m_iPointCount - 1; ++i)
		{
			m_tIn.segmentlist[2 * i + 0] = i + 0;
			m_tIn.segmentlist[2 * i + 1] = i + 1;
		}
		m_tIn.segmentlist[2 * (m_iPointCount - 1) + 0] = m_iPointCount - 1;
		m_tIn.segmentlist[2 * (m_iPointCount - 1) + 1] = 0;
	}
	//UpdateHoleList();
	//UpdateRegionList();

	_char szTriswitches[3] = "pz";
	triangulate(szTriswitches, &m_tIn, &m_tOut, nullptr);
	
	BakeNavMesh();

	return S_OK;
}

HRESULT CNavMeshView::Tick()
{
	Input();

	ImGui::Begin("NavMeshTool");

	InfoView();
	switch (m_eCurrentTriangleMode)
	{
	case TRIMODE::DEFAULT:
		PointsGroup();
		break;
	case TRIMODE::OBSTACLE:
		ObstaclePointsGroup();
		break;
	case TRIMODE::REGION:
		
		break;
	}
	//CellGroup();

	ImGui::End();

	return S_OK;
}

HRESULT CNavMeshView::LateTick()
{
	return S_OK;
}

HRESULT CNavMeshView::DebugRender()
{
	//DebugRenderLegacy();

	m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	if (!m_vecCells.empty())
	{
		for (auto cell = m_vecCells.begin(); cell != m_vecCells.end();)
		{
			if (nullptr != (*cell) && true == (*cell)->isDead)
			{
				Safe_Delete(*cell);
				cell = m_vecCells.erase(cell);
				continue;
			}

			Vec3 vP0 = (*cell)->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP1 = (*cell)->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP2 = (*cell)->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

			m_pBatch->Begin();
			if (true == (*cell)->isNew)
			{
				DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::Blue);
			}
			else
			{
				DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::LimeGreen);
			}
			m_pBatch->End();

			++cell;
		}
	}

	if (false == m_vecObstacles.empty())
	{
		m_pBatch->Begin();
		for (_int i = 0; i < m_vecObstacles.size(); ++i)
		{
			for (_int j = 0; j < m_vecObstacles[i].vecPoints.size() - 1; ++j)
			{
				Vec3 vLine1 =
				{
					m_vecObstacles[i].vecPoints[j].x,
					0.0f,
					m_vecObstacles[i].vecPoints[j].z
				};
				Vec3 vLine2 =
				{
					m_vecObstacles[i].vecPoints[j + 1].x,
					0.0f,
					m_vecObstacles[i].vecPoints[j + 1].z,
				};

				m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
			}

			Vec3 vLine1 =
			{
				m_vecObstacles[i].vecPoints[m_vecObstacles[i].vecPoints.size() - 1].x,
				0.0f,
				m_vecObstacles[i].vecPoints[m_vecObstacles[i].vecPoints.size() - 1].z

			};
			Vec3 vLine2 =
			{
				m_vecObstacles[i].vecPoints[0].x,
				0.0f,
				m_vecObstacles[i].vecPoints[0].z
			};

			m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
		}
		m_pBatch->End();
	}

	/*m_pBatch->Begin();
	DX::Draw(m_pBatch, m_tNavMeshBoundVolume, Colors::Green);
	m_pBatch->End();*/

	return S_OK;
}

void CNavMeshView::ClearNeighbors(vector<CellData*>& vecCells)
{
	for (auto cell : vecCells)
	{
		for (_int i = LINE_AB; i < LINE_END; ++i)
		{
			cell->arrNeighbors[i] = nullptr;
		}
	}
}

void CNavMeshView::SetUpNeighbors(vector<CellData*>& vecCells)
{
	for (_int sour = 0; sour < vecCells.size(); ++sour)
	{
		for (_int dest = 0; dest < vecCells.size(); ++dest)
		{
			if (vecCells[sour] == vecCells[dest])
			{
				continue;
			}

			if (true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_A], vecCells[sour]->vPoints[POINT_B]))
			{
				vecCells[sour]->arrNeighbors[LINE_AB] = vecCells[dest];
			}
			else if (true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_B], vecCells[sour]->vPoints[POINT_C]))
			{
				vecCells[sour]->arrNeighbors[LINE_BC] = vecCells[dest];
			}
			else if (true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_C], vecCells[sour]->vPoints[POINT_A]))
			{
				vecCells[sour]->arrNeighbors[LINE_CA] = vecCells[dest];
			}
		}
	}
}

HRESULT CNavMeshView::BakeNavMesh()
{
	if (false == m_vecCells.empty())
	{
		for (auto iter : m_vecCells)
		{
			delete iter;
		}

		m_vecCells.clear();
	}

	for (_int i = 0; i < m_tOut.numberoftriangles; ++i)
	{
		_int iIdx1 = m_tOut.trianglelist[i * 3 + POINT_A];
		_int iIdx2 = m_tOut.trianglelist[i * 3 + POINT_B];
		_int iIdx3 = m_tOut.trianglelist[i * 3 + POINT_C];

		Vec3 vtx[POINT_END] =
		{
			{ m_tOut.pointlist[iIdx1 * 2], 0.f, m_tOut.pointlist[iIdx1 * 2 + 1] },
			{ m_tOut.pointlist[iIdx2 * 2], 0.f, m_tOut.pointlist[iIdx2 * 2 + 1] },
			{ m_tOut.pointlist[iIdx3 * 2], 0.f, m_tOut.pointlist[iIdx3 * 2 + 1] },
		};

		CellData* pCellData = new CellData;
		pCellData->vPoints = { vtx[POINT_A], vtx[POINT_B], vtx[POINT_C] };
		pCellData->CW();

		m_vecCells.push_back(pCellData);
	}
	
	SetUpNeighbors(m_vecCells);

	return S_OK;
}

HRESULT CNavMeshView::BakeNavMeshLegacy()
{
	if (false == m_vecCells.empty())
	{
		for (auto iter : m_vecCells)
		{
			Safe_Delete(iter);
		}

		m_vecCells.clear();
	}

	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	map<LAYERTAG, class CLayer*>::iterator iter = mapLayers.find(LAYERTAG::GROUND);
	if (iter == mapLayers.end())
	{
		return S_OK;
	}

	vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();

	vector<CellData*> vecCellCache;

	//vecCellCache
	for (auto iter : vecObjects)
	{
		CModel* pModel = iter->GetModel();

		if (nullptr == pModel)
		{
			continue;
		}

		vector<Vec3>& vecSurfaceVtx = pModel->GetSurfaceVtx();
		vector<FACEINDICES32>& vecSurfaceIdx = pModel->GetSurfaceIdx();

		for (auto idx : vecSurfaceIdx)
		{
			Vec3 vtx[POINT_END] =
			{
				Vec3::Transform(vecSurfaceVtx[idx._0], iter->GetTransform()->WorldMatrix()),
				Vec3::Transform(vecSurfaceVtx[idx._1], iter->GetTransform()->WorldMatrix()),
				Vec3::Transform(vecSurfaceVtx[idx._2], iter->GetTransform()->WorldMatrix()),
			};

			// Check Bounding Volume
			_bool isOut = false;

			for (_int i = POINT_A; i < POINT_END; ++i)
			{
				if (ContainmentType::DISJOINT == m_tNavMeshBoundVolume.Contains(vtx[i]))
				{
					isOut = true;
					break;
				}
			}

			if (true == isOut)
			{
				continue;
			}

			CellData* pCellData = new CellData;
			pCellData->vPoints[POINT_A] = vtx[0];
			pCellData->vPoints[POINT_B] = vtx[1];
			pCellData->vPoints[POINT_C] = vtx[2];

			Vec3 vtxAB = vtx[POINT_B] - vtx[POINT_A];
			Vec3 vtxBC = vtx[POINT_C] - vtx[POINT_B];
			Vec3 vResult = vtxAB.Cross(vtxBC);

			vResult.Normalize();

			Vec3 vFloor(vResult.x, 0.0f, vResult.z);
			vFloor.Normalize();

			_float fDegree = XMConvertToDegrees(acosf(vResult.Dot(vFloor)));

			if (0.0f > vResult.y)
			{
				fDegree *= -1.0f;

				pCellData->iSlope = (_int)fDegree;
				pCellData->isOverSlope = true;
			}
			else if (90.f - m_fSlopeDegree >= (_int)fDegree)
			{	// Slope 이하인 Cell 기록.
				pCellData->iSlope = (_int)fDegree;
				pCellData->isOverSlope = true;
			}			

			vecCellCache.push_back(pCellData);
		}
	}

	SetUpNeighbors(vecCellCache);

	// Max Climb
	// for (auto cell : vecCellCache)
	for (_int k = 0; k < vecCellCache.size(); ++k)
	{
		if (true == vecCellCache[k]->isOverSlope)
		{	// Slope 초과인 경우
			for (_int i = LINE_AB; i < LINE_END; ++i)
			{
				if (nullptr != vecCellCache[k]->arrNeighbors[i] && false == vecCellCache[k]->arrNeighbors[i]->isOverSlope)
				{	// Neighbor 중에 Slope 이하인 Mesh 있다면 높이 계산 후 Max Climb이하면 vecCells에 저장.
					Vec3 vSharedLine = vecCellCache[k]->vPoints[(i + 1) % 3] - vecCellCache[k]->vPoints[i];
					Vec3 vAnotherLine = vecCellCache[k]->vPoints[(i + 2) % 3] - vecCellCache[k]->vPoints[i];

					_float fSlopeLength = vAnotherLine.Cross(vSharedLine).Length() / vSharedLine.Length();
					_float fSlopeHeight = fabs(fSlopeLength * sinf(XMConvertToRadians(vecCellCache[k]->iSlope)));

					if (fSlopeHeight + m_fEpsilon <= m_fMaxClimb)
					{
						m_vecCells.push_back(vecCellCache[k]);
						break;
					}
				}
			}
		}
		else
		{
			m_vecCells.push_back(vecCellCache[k]);
		}
	}

	ClearNeighbors(m_vecCells);
	SetUpNeighbors(m_vecCells);
	::swap(vecCellCache, m_vecCells);
	m_vecCells.clear();

	//for (auto cell : vecCellCache)
	for (_int k = 0; k < vecCellCache.size(); ++k)
	{
		_int iNumNeighbor = 0;

		for (_int i = LINE_AB; i < LINE_END; ++i)
		{
			if (nullptr != vecCellCache[k]->arrNeighbors[i])
			{
				++iNumNeighbor;
			}
		}

		if (true != vecCellCache[k]->isOverSlope || 1 != iNumNeighbor)
		{
			m_vecCells.push_back(vecCellCache[k]);
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdatePointList()
{
	m_tIn.numberofpoints = m_vecPoints.size();
	if (0 < m_tIn.numberofpoints)
	{
		SAFE_REALLOC(TRI_REAL, m_tIn.pointlist, m_tIn.numberofpoints * 2)

		for (_int i = 0; i < m_vecPoints.size(); ++i)
		{
			m_tIn.pointlist[2 * i + 0] = m_vecPoints[i].x;
			m_tIn.pointlist[2 * i + 1] = m_vecPoints[i].z;
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateSegmentList()
{
	m_tIn.numberofsegments = m_vecPoints.size();
	if (0 < m_tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, m_tIn.segmentlist, m_tIn.numberofsegments * 2)

		// Points
		for (_int i = 0; i < m_iPointCount - 1; ++i)
		{
			m_tIn.segmentlist[2 * i + 0] = i + 0;
			m_tIn.segmentlist[2 * i + 1] = i + 1;
		}
		m_tIn.segmentlist[2 * (m_iPointCount - 1) + 0] = m_iPointCount - 1;
		m_tIn.segmentlist[2 * (m_iPointCount - 1) + 1] = 0;

		//// Obstacles
		//for (_int j = 0; j < m_vecObstacles.size(); ++j)
		//{
		//	for (_int i = 0; i < m_vecObstacles[j].numberof - 1; ++i)
		//	{
		//		m_tIn.segmentlist[m_vecObstacles[j].start + 2 * i + 0] = m_vecObstacles[j].start / 2 + i + 0;
		//		m_tIn.segmentlist[m_vecObstacles[j].start + 2 * i + 1] = m_vecObstacles[j].start / 2 + i + 1;
		//	}
		//	m_tIn.segmentlist[m_vecObstacles[j].start + 2 * (m_vecObstacles[j].numberof - 1) + 0] = m_vecObstacles[j].start / 2 + m_vecObstacles[j].numberof - 1;
		//	m_tIn.segmentlist[m_vecObstacles[j].start + 2 * (m_vecObstacles[j].numberof - 1) + 1] = m_vecObstacles[j].start / 2;
		//}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateHoleList()
{
	m_tIn.numberofholes = m_vecObstacles.size();
	if (0 < m_tIn.numberofholes)
	{
		SAFE_REALLOC(TRI_REAL, m_tIn.holelist, m_tIn.numberofholes * 2)

		for (_int i = 0; i < m_tIn.numberofholes; ++i)
		{
			m_tIn.holelist[2 * i + 0] = m_vecObstacles[i].center.x;
			m_tIn.holelist[2 * i + 1] = m_vecObstacles[i].center.z;
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateRegionList()
{
	m_tIn.numberofregions = m_vecRegions.size();
	if (0 < m_tIn.numberofregions)
	{
		SAFE_REALLOC(TRI_REAL, m_tIn.regionlist, m_tIn.numberofregions * 4)

			for (_int i = 0; i < m_vecRegions.size(); ++i)
			{
				m_tIn.regionlist[4 * i + 0] = m_vecRegions[i].x;
				m_tIn.regionlist[4 * i + 1] = m_vecRegions[i].z;
				m_tIn.regionlist[4 * i + 1] = 0.f;
				m_tIn.regionlist[4 * i + 1] = 0.f;
			}
	}

	return S_OK;
}

HRESULT CNavMeshView::DynamicUpdate(const Obst& tObst)
{
	set<CellData*> setIntersected;
	map<Vec3, pair<Vec3, CellData*>> mapOutlineCells;

	GetIntersectedCells(tObst, setIntersected);

	for (auto tCell : setIntersected)
	{
		for (uint8 i = 0; i < LINE_END; ++i)
		{	// neighbor가 유효한 edge 추출 
			if (setIntersected.end() == setIntersected.find(tCell->arrNeighbors[i]))
			{	// 해당 edge는 outline.
				if (mapOutlineCells.end() != mapOutlineCells.find(tCell->vPoints[i]))
				{
					_int a = 0;
				}
				else
				{
					mapOutlineCells.emplace(tCell->vPoints[i], pair(tCell->vPoints[(i + 1) % POINT_END], tCell->arrNeighbors[i]));
				}
			}
		}
	}

	vector<Vec3> vecOutlineCW;	// 시계 방향 정렬
	vecOutlineCW.push_back(mapOutlineCells.begin()->first);

	while (vecOutlineCW.size() < mapOutlineCells.size())
	{
		auto pair = mapOutlineCells.find(*(vecOutlineCW.end() - 1));
		if (mapOutlineCells.end() == pair)
		{
			_int a = 0;
		}
		else
		{
			vecOutlineCW.push_back(pair->second.first);
		}
	}

	for (auto tCell : setIntersected)
	{
		tCell->isDead = true;
	}

	triangulateio tIn = { 0 }, tOut = { 0 };

#pragma region pointlist
	tIn.numberofpoints = vecOutlineCW.size() + tObst.vecPoints.size();
	if (0 < tIn.numberofpoints)
	{
		SAFE_REALLOC(TRI_REAL, tIn.pointlist, tIn.numberofpoints * 2)

		_int i = 0;
		for (auto point : vecOutlineCW)
		{
			tIn.pointlist[2 * i + 0] = point.x;
			tIn.pointlist[2 * i + 1] = point.z;
			++i;
		}

		// tObst
		for (_int j = 0; j < tObst.vecPoints.size(); ++j)
		{
			tIn.pointlist[2 * (i + j) + 0] = tObst.vecPoints[j].x;
			tIn.pointlist[2 * (i + j) + 1] = tObst.vecPoints[j].z;
		}
	}
#pragma endregion pointlist

#pragma region segmentlist
	tIn.numberofsegments = vecOutlineCW.size() + tObst.vecPoints.size();
	if (0 < tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, tIn.segmentlist, tIn.numberofsegments * 2)

		// Points
		for (_int i = 0; i < vecOutlineCW.size() - 1; ++i)
		{
			tIn.segmentlist[2 * i + 0] = i + 0;
			tIn.segmentlist[2 * i + 1] = i + 1;
		}
		tIn.segmentlist[2 * (vecOutlineCW.size() - 1) + 0] = vecOutlineCW.size() - 1;
		tIn.segmentlist[2 * (vecOutlineCW.size() - 1) + 1] = 0;

		// Obstacles
		_int iStartIndex = 2 * vecOutlineCW.size();
		for (_int i = 0; i < tObst.vecPoints.size() - 1; ++i)
		{
			tIn.segmentlist[iStartIndex + 2 * i + 0] = iStartIndex / 2 + i + 0;
			tIn.segmentlist[iStartIndex + 2 * i + 1] = iStartIndex / 2 + i + 1;
		}

		tIn.segmentlist[iStartIndex + 2 * (tObst.vecPoints.size() - 1) + 0] = iStartIndex / 2 + tObst.vecPoints.size() - 1;
		tIn.segmentlist[iStartIndex + 2 * (tObst.vecPoints.size() - 1) + 1] = iStartIndex / 2;
	}
#pragma endregion segmentlist

#pragma region holelist
	tIn.numberofholes = 1;
	if (0 < tIn.numberofholes)
	{
		SAFE_REALLOC(TRI_REAL, tIn.holelist, tIn.numberofholes * 2)

		for (_int i = 0; i < tIn.numberofholes; ++i)
		{
			tIn.holelist[2 * i + 0] = tObst.center.x;
			tIn.holelist[2 * i + 1] = tObst.center.z;
		}
	}
#pragma endregion holelist

	tIn.numberofregions = 0;

	_char szTriswitches[3] = "pz";
	triangulate(szTriswitches, &tIn, &tOut, nullptr);

	// 부분 메쉬 생성.
	vector<CellData*> vecNewCells;

	for (_int i = 0; i < tOut.numberoftriangles; ++i)
	{
		_int iIdx1 = tOut.trianglelist[i * 3 + POINT_A];
		_int iIdx2 = tOut.trianglelist[i * 3 + POINT_B];
		_int iIdx3 = tOut.trianglelist[i * 3 + POINT_C];

		Vec3 vtx[POINT_END] =
		{
			{ tOut.pointlist[iIdx1 * 2], 0.f, tOut.pointlist[iIdx1 * 2 + 1] },
			{ tOut.pointlist[iIdx2 * 2], 0.f, tOut.pointlist[iIdx2 * 2 + 1] },
			{ tOut.pointlist[iIdx3 * 2], 0.f, tOut.pointlist[iIdx3 * 2 + 1] },
		};

		CellData* pCellData = new CellData;
		pCellData->vPoints = { vtx[POINT_A], vtx[POINT_B], vtx[POINT_C] };
		pCellData->CW();

		vecNewCells.push_back(pCellData);
	}

	SetUpNeighbors(vecNewCells);

	for (auto cell : vecNewCells)
	{	// 재구성된 데이터를 다시 전체 맵에 연결.
		for (uint8 i = LINE_AB; i < LINE_END; ++i)
		{
			if (nullptr == cell->arrNeighbors[i])
			{	// new cell의 outline은 neighbor가 없음.
				auto OutCell = mapOutlineCells.find(cell->vPoints[i]);
				if (mapOutlineCells.end() != OutCell)
				{
					if (cell->vPoints[(i + 1) % POINT_END] == OutCell->second.first)
					{	// mapOutlineCells에서 outline 찾았다면 상호 연결
						cell->arrNeighbors[i] = OutCell->second.second;

						if (nullptr == OutCell->second.second || OutCell->second.second == cell)
						{
							continue;
						}

						if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_A], OutCell->second.second->vPoints[POINT_B]))
						{
							OutCell->second.second->arrNeighbors[LINE_AB] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_B], OutCell->second.second->vPoints[POINT_C]))
						{
							OutCell->second.second->arrNeighbors[LINE_BC] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_C], OutCell->second.second->vPoints[POINT_A]))
						{
							OutCell->second.second->arrNeighbors[LINE_CA] = cell;
						}						
					}
				}
			}
		}

		cell->isNew = true;
		m_vecCells.push_back(cell);
	}

	return S_OK;
}

HRESULT CNavMeshView::DebugRenderLegacy()
{
	m_pEffect->SetWorld(Matrix::Identity);

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	if (false == m_vecPointSpheres.empty())
	{
		for (auto& iter : m_vecPointSpheres)
		{
			BoundingSphere tS(iter.Center + 0.05f * Vec3::UnitY, 0.5f);
			DX::Draw(m_pBatch, tS, Colors::LimeGreen);
		}
	}

	if (false == m_vecObstaclePointSpheres.empty())
	{
		for (auto& iter : m_vecObstaclePointSpheres)
		{
			BoundingSphere tS(iter.Center + 0.05f * Vec3::UnitY, 0.5f);
			DX::Draw(m_pBatch, tS, Colors::Red);
		}
	}

	if (false == m_vecRegionSpheres.empty())
	{
		for (auto& iter : m_vecRegionSpheres)
		{
			BoundingSphere tS(iter.Center + 0.05f * Vec3::UnitY, 0.5f);
			DX::Draw(m_pBatch, tS, Colors::Blue);
		}
	}

	m_pBatch->End();

	// triangle::DT
	if (FAILED(RenderDT()))
		return E_FAIL;

	// triangle::VD
	//if (FAILED(RenderVD()))
	//	return E_FAIL;

	/*if (2 == m_vecPoints.size())
	{
		VertexPositionColor verts[2];

		_float3 vP0 = m_vecPoints[0] + Vec3(0.f, 0.05f, 0.f);
		_float3 vP1 = m_vecPoints[1] + Vec3(0.f, 0.05f, 0.f);

		XMStoreFloat3(&verts[0].position, XMLoadFloat3(&vP0));
		XMStoreFloat3(&verts[1].position, XMLoadFloat3(&vP1));

		XMStoreFloat4(&verts[0].color, Colors::Cyan);
		XMStoreFloat4(&verts[1].color, Colors::Cyan);

		m_pBatch->Begin();
		m_pBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
		m_pBatch->End();
	}

	if (!m_vecCells.empty())
	{
		for (_int i = 0; i < m_vecCells.size(); ++i)
		{
			Vec3 vP0 = m_vecCells[i]->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP1 = m_vecCells[i]->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP2 = m_vecCells[i]->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

			m_pBatch->Begin();
			DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::Cyan);
			m_pBatch->End();
		}

		Vec3 vP0 = m_vecCells[m_Item_Current]->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
		Vec3 vP1 = m_vecCells[m_Item_Current]->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
		Vec3 vP2 = m_vecCells[m_Item_Current]->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

		m_pBatch->Begin();
		DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::Coral);
		m_pBatch->End();
	}*/

	/*if (1 < m_vecPoints.size())
	{
		for (_int i = 0; i < m_vecPoints.size() - 1; ++i)
		{
			m_pBatch->Begin();
			m_pBatch->DrawLine(VertexPositionColor(m_vecPoints[i], Colors::Lime), VertexPositionColor(m_vecPoints[i + 1], Colors::Lime));
			m_pBatch->End();
		}
	}*/

	return S_OK;
}

HRESULT CNavMeshView::RenderDT()
{
	if (0 < m_tOut.numberoftriangles)
	{
		m_pBatch->Begin();
		for (_int i = 0; i < m_tOut.numberoftriangles; ++i)
		{
			_int iIdx1 = m_tOut.trianglelist[i * 3 + POINT_A];
			_int iIdx2 = m_tOut.trianglelist[i * 3 + POINT_B];
			_int iIdx3 = m_tOut.trianglelist[i * 3 + POINT_C];

			Vec3 vTri1 = { m_tOut.pointlist[iIdx1 * 2], 0.f, m_tOut.pointlist[iIdx1 * 2 + 1] };
			Vec3 vTri2 = { m_tOut.pointlist[iIdx2 * 2], 0.f, m_tOut.pointlist[iIdx2 * 2 + 1] };
			Vec3 vTri3 = { m_tOut.pointlist[iIdx3 * 2], 0.f, m_tOut.pointlist[iIdx3 * 2 + 1] };

			DX::DrawTriangle(m_pBatch, vTri1, vTri2, vTri3, Colors::LimeGreen);
		}
		m_pBatch->End();
	}
	
	if (false == m_vecObstacles.empty())
	{
		m_pBatch->Begin();
		for (_int i = 0; i < m_vecObstacles.size(); ++i)
		{
			for (_int j = 0; j < m_vecObstacles[i].vecPoints.size() - 1; ++j)
			{
				Vec3 vLine1 =
				{
					m_vecObstacles[i].vecPoints[j].x,
					0.0f,
					m_vecObstacles[i].vecPoints[j].z,
				};
				Vec3 vLine2 =
				{
					m_vecObstacles[i].vecPoints[j + 1].x,
					0.0f,
					m_vecObstacles[i].vecPoints[j + 1].z,
				};

				m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
			}

			Vec3 vLine1 =
			{
				m_vecObstacles[i].vecPoints[m_vecObstacles[i].vecPoints.size() - 1].x,
				0.0f,
				m_vecObstacles[i].vecPoints[m_vecObstacles[i].vecPoints.size() - 1].z
			};
			Vec3 vLine2 =
			{
				m_vecObstacles[i].vecPoints[0].x,
				0.0f,
				m_vecObstacles[i].vecPoints[0].z,
			};

			m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
		}
		m_pBatch->End();
	}

	return S_OK;
}

HRESULT CNavMeshView::RenderVD()
{
	if (0 < m_tVD_out.numberofedges)
	{
		m_pBatch->Begin();
		for (_int i = 0; i < m_tVD_out.numberofedges; ++i)
		{
			_int iIdx1 = m_tVD_out.edgelist[i * 2 + POINT_A];
			_int iIdx2 = m_tVD_out.edgelist[i * 2 + POINT_B];

			if (0 <= iIdx1 && 0 <= iIdx2)
			{
				Vec4 vLine1 = { m_tVD_out.pointlist[iIdx1 * 2], 0.f, m_tVD_out.pointlist[iIdx1 * 2 + 1], 1.f };
				Vec4 vLine2 = { m_tVD_out.pointlist[iIdx2 * 2], 0.f, m_tVD_out.pointlist[iIdx2 * 2 + 1], 1.f };

				m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Cyan), VertexPositionColor(vLine2, Colors::Cyan));
			}
		}
		m_pBatch->End();
	}

	return S_OK;
}

void CNavMeshView::SetPolygonHoleCenter(Obst& tObst)
{
	_int iSize = tObst.vecPoints.size();

	for (_int i = 0; i < iSize; ++i)
	{
		Vec3 vCenter = Vec3::Zero;

		for(_int j = 0; j < 3; ++j)
		{
			vCenter.x += tObst.vecPoints[(i + j) % iSize].x;
			vCenter.z += tObst.vecPoints[(i + j) % iSize].z;
		}

		vCenter /= 3.f;

		// RayCast
		_int iCrosses = 0;

		for (_int m = 0; m < iSize; ++m)
		{
			_float fSourX = tObst.vecPoints[m % iSize].x;
			_float fSourZ = tObst.vecPoints[m % iSize].z;

			_float fDestX = tObst.vecPoints[(m + 1) % iSize].x;
			_float fDestZ = tObst.vecPoints[(m + 1) % iSize].z;

			if ((fSourX > vCenter.x) != (fDestX > vCenter.x))	// x 좌표 검사
			{
				_float fAtZ = (fDestZ - fSourZ) * (vCenter.x - fSourX) / (fDestX - fSourX) + fSourZ; 

				if (vCenter.z < fAtZ)	// z 좌표 검사
				{
					++iCrosses;
				}
			}
		}

		if (0 < iCrosses % 2)
		{
			tObst.center = vCenter;
			break;
		}
	}
}

void CNavMeshView::GetIntersectedCells(const Obst& tObst, OUT set<CellData*>& setIntersected)
{
	for (auto cell : m_vecCells)
	{
		if (true == tObst.AABB.Intersects(cell->vPoints[POINT_A], cell->vPoints[POINT_B], cell->vPoints[POINT_C]))
		{
			setIntersected.emplace(cell);
		}
	}
}

void CNavMeshView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	
	const POINT& p = CGameInstance::GetInstance()->GetMousePos();
	if (p.x > 1440 || p.x < 0 || p.y > 810 || p.y < 0)
		return;

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		if (m_IsPickingActivated)
		{
			const POINT& p = m_pGameInstance->GetMousePos();
			Pick(p.x, p.y);
		}
	}
}

_bool CNavMeshView::Pick(_uint screenX, _uint screenY)
{
	map<LAYERTAG, CLayer*>& mapLayer = m_pGameInstance->GetCurrentLevelLayers();
	auto iter = mapLayer.find(LAYERTAG::GROUND);
	vector<CGameObject*>* vecGroundObjects = nullptr;

	if (mapLayer.end() != iter)
	{
		vecGroundObjects = &iter->second->GetGameObjects();
	}

	if (nullptr != vecGroundObjects && true == vecGroundObjects->empty())
	{
		//return false;
	}

	Viewport& vp = m_pGameInstance->GetViewPort();
	const Matrix& P = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	const Matrix& V = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);

	Vec3 pickPos;
	_float fDistance = FLT_MAX;
	_float fMinDistance = FLT_MAX;

	_float fWidth = vp.width;
	_float fHeight = vp.height;

	_float viewX = (+2.0f * screenX / fWidth - 1.0f) / P(0, 0);
	_float viewY = (-2.0f * screenY / fHeight + 1.0f) / P(1, 1);

	const Matrix& VI = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	Vec4 vRayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4 vRayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

	Vec3 vWorldRayOrigin = XMVector3TransformCoord(vRayOrigin, VI);
	Vec3 vWorldRayDir = XMVector3TransformNormal(vRayDir, VI);
	vWorldRayDir.Normalize();

	//////////////////////////////////////////
	// 아래는 코드 분할 및 정리 무조건 필요!!! //
	//////////////////////////////////////////

	CGameObject* pObject = nullptr;

	if (nullptr == vecGroundObjects)
	{
		goto TERRAIN_PICKED;
	}

	for (auto& iter : *vecGroundObjects)
	{
		vector<Vec3>& vecSurfaceVtx = iter->GetModel()->GetSurfaceVtx();
		vector<FACEINDICES32>& vecSurfaceIdx = iter->GetModel()->GetSurfaceIdx();

		const Matrix& W = iter->GetTransform()->WorldMatrix();
		Vec3 n = vp.Unproject(Vec3(screenX, screenY, 0), P, V, W);
		Vec3 f = vp.Unproject(Vec3(screenX, screenY, 1), P, V, W);

		Vec3 start = n;
		Vec3 direction = f - n;
		direction.Normalize();

		Ray cRay(start, direction);

		for (_int i = 0; i < vecSurfaceIdx.size(); i++)
		{
			if (cRay.Intersects(vecSurfaceVtx[vecSurfaceIdx[i]._0], vecSurfaceVtx[vecSurfaceIdx[i]._1], vecSurfaceVtx[vecSurfaceIdx[i]._2], OUT fDistance))
			{
				Vec3 vPickedPos = cRay.position + cRay.direction * fDistance;
				if (isnan(vPickedPos.x) || isnan(vPickedPos.y) || isnan(vPickedPos.z) || isnan(fDistance))
					continue;

				if (fDistance < fMinDistance)
				{
					fMinDistance = fDistance;
					pickPos = vPickedPos;
					pObject = iter;
				}
			}
		}
	}

	if (nullptr == pObject)
	{
		const Matrix& W = pObject->GetTransform()->WorldMatrix();
		pickPos = XMVector3TransformCoord(pickPos, W);
	}
	else
	{	// TODO : 여기까지 코드 정리 필요
	TERRAIN_PICKED:
		const POINT& p = m_pGameInstance->GetMousePos();
		m_pTerrainBuffer->Pick(p.x, p.y, pickPos, fDistance, m_pTerrainBuffer->GetTransform()->WorldMatrix());
	}
	
	BoundingSphere tSphere;
	tSphere.Center = pickPos;
	tSphere.Radius = 2.f;

	if (TRIMODE::DEFAULT == m_eCurrentTriangleMode)
	{
		m_vecPoints.push_back(pickPos);
		m_vecPointSpheres.push_back(tSphere);
		s2cPushBack(m_strPoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));
		++m_iPointCount;

		if (3 <= m_vecPoints.size())
		{
			SafeReleaseTriangle(m_tOut);

			UpdatePointList();
			UpdateSegmentList();
			UpdateHoleList();
			UpdateRegionList();

			static _char triswitches[3] = "pz";
			triangulate(triswitches, &m_tIn, &m_tOut, nullptr);
		}
	}
	else if (TRIMODE::OBSTACLE == m_eCurrentTriangleMode)
	{
		m_vecPoints.push_back(pickPos);
		m_vecObstaclePoints.push_back(pickPos);
		m_vecObstaclePointSpheres.push_back(tSphere);
		s2cPushBack(m_strObstaclePoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));
	}
	else if (TRIMODE::REGION == m_eCurrentTriangleMode)
	{
		m_vecRegions.push_back(pickPos);
		m_vecRegionSpheres.push_back(tSphere);
		//s2cPushBack(m_strRegions, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));
	}



	// 삼각형 만드는게 아니라 점만 찍도록.
	//if (3 == m_vecPoints.size())
	/*{
		CellData* tCellData = new CellData;
		tCellData->vPoints[0] = m_vecPoints[0];
		tCellData->vPoints[1] = m_vecPoints[1];
		tCellData->vPoints[2] = m_vecPoints[2];
		tCellData->CW();
		m_vecCells.push_back(tCellData);

		m_vecPoints.clear();
		m_strPoints.clear();

		s2cPushBack(m_strCells, to_string(m_vecCells.size() - 1));
	}
	else
		s2cPushBack(m_strPoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));*/

	return true;
}

HRESULT CNavMeshView::Save()
{
	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/NavMesh/") + m_strFilePath + TEXT(".nav"), Write);

	for (auto& iter : m_vecCells)
		file->Write(*iter);

	return S_OK;
}

HRESULT CNavMeshView::Load()
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/NavMesh/") + m_strFilePath + TEXT(".nav"), Read);

	while (true)
	{
		CellData* tCellData = new CellData;

		if (false == file->Read(*tCellData))
			break;
		
		for (_int i = 0; i < 3; ++i)
		{

			BoundingSphere tSphere;
			tSphere.Center = tCellData->vPoints[i];
			tSphere.Radius = 1.f;
			m_vecPointSpheres.push_back(tSphere);
		}
		m_vecCells.push_back(tCellData);
		s2cPushBack(m_strCells, to_string(m_vecCells.size() - 1));
	}

	return S_OK;
}

void CNavMeshView::InfoView()
{
	ImGui::Text("This window has some useful function for Objects in Level.");
	ImGui::NewLine();

	ImGui::Text("Mouse Picking ");
	if (m_IsPickingActivated)
	{
		ImGui::SameLine();
		if (ImGui::Button("Deactivate"))
			m_IsPickingActivated = false;
	}
	else
	{
		ImGui::SameLine();
		if (ImGui::Button("Activate"))
		{
			m_IsPickingActivated = true;
			m_pMediator->OnNotifiedPickingOn(this);
		}
	}
	ImGui::NewLine();

	if (ImGui::BeginCombo(" ", m_strCurrentTriangleMode.c_str()))
	{
		static const _char* szMode[3] = { "Default", "Obstacle", "Region" };

		for (uint8 i = 0; i < (uint8)TRIMODE::MODE_END; ++i)
		{
			_bool isSelected = (0 == strcmp(m_strCurrentTriangleMode.c_str(), szMode[i]));
			
			if (ImGui::Selectable(szMode[i], isSelected))
			{
				m_strCurrentTriangleMode = szMode[i];
				m_eCurrentTriangleMode = (TRIMODE)i;
				m_Point_Current = 0;
			}

			if (true == isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::NewLine();

	/*ImGui::InputFloat("SlopeMax (degree)", &m_fSlopeDegree);
	ImGui::InputFloat("ClimbMax (height)", &m_fMaxClimb);
	ImGui::InputFloat("AreaMin (degree)", &m_fMinArea);
	ImGui::InputFloat3("AABB Center", (_float*)&m_tNavMeshBoundVolume.Center);
	ImGui::InputFloat3("AABB Extent", (_float*)&m_tNavMeshBoundVolume.Extents);*/

	if (ImGui::Button("BakeNav"))
	{
		BakeNavMesh();
	}ImGui::SameLine();
	
	if (ImGui::Button("SaveNav"))
	{
		Save();
	}ImGui::SameLine();

	if (ImGui::Button("LoadNav"))
	{
		Load();
	}
}

void CNavMeshView::PointsGroup()
{
	ImGui::ListBox("Points", &m_Point_Current, m_strPoints.data(), m_strPoints.size(), 3);
	if (ImGui::Button("PopBackPoint"))
	{
		if (m_vecPoints.empty())
			return;

		m_strPoints.pop_back();
		m_vecPoints.pop_back();
		m_vecPointSpheres.pop_back();
	}
}

void CNavMeshView::ObstaclePointsGroup()
{
	ImGui::ListBox("ObstaclePoints", &m_Point_Current, m_strObstaclePoints.data(), m_strObstaclePoints.size(), 3);
	
	if (ImGui::Button("PopBackObstaclePoint"))
	{
		if (m_strObstaclePoints.empty())
			return;

		m_vecPoints.pop_back();
		m_strObstaclePoints.pop_back();
		m_vecObstaclePoints.pop_back();
		m_vecObstaclePointSpheres.pop_back();
	}

	if (false == m_vecObstaclePoints.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("CreateObstacle"))
		{
			//SafeReleaseTriangle(m_tOut);
			//SafeReleaseTriangle(m_tVD_out);

			UpdatePointList();
			
			{
				Obst tObst;

				TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
				for (auto vPoint : m_vecObstaclePoints)
				{
					if (fMaxX < vPoint.x) fMaxX = vPoint.x;
					if (fMinX > vPoint.x) fMinX = vPoint.x;

					if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
					if (fMinZ > vPoint.z) fMinZ = vPoint.z;

					tObst.vecPoints.push_back(vPoint);
				}

				//_int iStartIndex = 2 * (m_vecPoints.size() - m_vecObstaclePoints.size());
				//_int iNumberOfPoints = m_vecObstaclePoints.size();
				Vec3 vAABBCenter((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
				Vec3 vAABBExtent((fMaxX - fMinX) * 0.5f, 10.0f, (fMaxZ - fMinZ) * 0.5f);
				tObst.AABB = BoundingBox(vAABBCenter, vAABBExtent);

				SetPolygonHoleCenter(tObst);
				
				m_vecObstacles.push_back(tObst);
				
				for (auto cell : m_vecCells)
				{
					if (true == cell->isNew)
					{
						cell->isNew = false;
					}
				}
				//
				DynamicUpdate(tObst);
				//
			}

			//UpdateSegmentList();
			//UpdateHoleList();

			//static _char triswitches[3] = "pz";
			//triangulate(triswitches, &m_tIn, &m_tOut, nullptr);

			for_each(m_strObstaclePoints.begin(), m_strObstaclePoints.end(), [](const _char* szPoint) { delete szPoint; });
			m_strObstaclePoints.clear();
			m_vecObstaclePoints.clear();
		}
	}
}
// https://gdcvault.com/play/1014514/AI-Navigation-It-s-Not
void CNavMeshView::CellGroup()
{
	ImGui::ListBox("Cells", &m_Item_Current, m_strCells.data(), m_strCells.size(), 3);
	if (ImGui::Button("PopBackCell"))
	{
		if (m_vecCells.empty())
			return;

		m_strCells.pop_back();
		m_vecCells.pop_back();

		auto iter = m_vecPointSpheres.end() - 3 - m_vecPoints.size();
		for(_int i = 0; i < 3; ++i)
			iter = m_vecPointSpheres.erase(iter);
	}
}

HRESULT CNavMeshView::SafeReleaseTriangle(triangulateio& tTriangle)
{
	if (tTriangle.pointlist)				{ free(tTriangle.pointlist);				tTriangle.pointlist = nullptr; }
	if (tTriangle.pointattributelist)		{ free(tTriangle.pointattributelist);		tTriangle.pointattributelist = nullptr; }
	if (tTriangle.pointmarkerlist)			{ free(tTriangle.pointmarkerlist);			tTriangle.pointmarkerlist = nullptr; }
	if (tTriangle.triangleattributelist)	{ free(tTriangle.triangleattributelist);	tTriangle.triangleattributelist = nullptr; }
	if (tTriangle.trianglearealist)			{ free(tTriangle.trianglearealist);			tTriangle.trianglearealist = nullptr; }
	if (tTriangle.trianglelist)				{ free(tTriangle.trianglelist);				tTriangle.trianglelist = nullptr; }
	if (tTriangle.neighborlist)				{ free(tTriangle.neighborlist);				tTriangle.neighborlist = nullptr; }
	if (tTriangle.segmentlist)				{ free(tTriangle.segmentlist);				tTriangle.segmentlist = nullptr; }
	if (tTriangle.segmentmarkerlist)		{ free(tTriangle.segmentmarkerlist);		tTriangle.segmentmarkerlist = nullptr; }
	//if (tTriangle.holelist)					{ free(tTriangle.holelist);					tTriangle.holelist = nullptr; }
	//if (tTriangle.regionlist)				{ free(tTriangle.regionlist);				tTriangle.regionlist = nullptr; }
	if (tTriangle.edgelist)					{ free(tTriangle.edgelist);					tTriangle.edgelist = nullptr; }
	if (tTriangle.edgemarkerlist)			{ free(tTriangle.edgemarkerlist);			tTriangle.edgemarkerlist = nullptr; }
	if (tTriangle.normlist)					{ free(tTriangle.normlist);					tTriangle.normlist = nullptr; }

	return S_OK;
}

CNavMeshView* CNavMeshView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CNavMeshView* pInstance = new CNavMeshView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CNavMeshView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavMeshView::Free()
{
	Super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);

	Safe_Release(m_pInputLayout);

	SafeReleaseTriangle(m_tIn);
	SafeReleaseTriangle(m_tOut);
	//SafeReleaseTriangle(m_tVD_out);
}