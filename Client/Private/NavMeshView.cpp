#include "stdafx.h"
#include "NavMeshView.h"
#include "ViewMediator.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "FileUtils.h"
#include <filesystem>
#include "Utils.h"
#include "Layer.h"
#include "DebugDraw.h"
#include "StructuredBuffer.h"
#include "Client_Macro.h"
#include "tinyxml2.h"
#include "Agent.h"
#include "CellData.h"
#include "Obstacle.h"

namespace fs = std::filesystem;

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

	if (FAILED(LoadObstacleOutlineData()))
		return E_FAIL;

	if (FAILED(InitialSetting()))
		return E_FAIL;

	if (FAILED(BakeNavMesh()))
		return E_FAIL;

	if (FAILED(RefreshNvFile()))
		return E_FAIL;

	m_pCS_TriTest = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(nullptr, LEVEL_STATIC, TEXT("Prototype_Component_Shader_TriangleTest"), nullptr));
	if (nullptr == m_pCS_TriTest)
		return  E_FAIL;

	//m_pCS_TriTest = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_TriangleTest.hlsl"), nullptr, 0);

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
		ObstaclesGroup();
		break;
	case TRIMODE::REGION:

		break;
	}
	ImGui::NewLine();

	if (ImGui::Button("CreateAgent"))
	{
		if (nullptr != m_pAgent)
		{			
			m_pGameInstance->DeleteObject(m_pAgent);
		}

		if (FAILED(CreateAgent(Vec3::Zero)))
		{
			ImGui::End();
			return E_FAIL;
		}
	}
	ImGui::NewLine();

	// stress test
	const _char* szStressButon = (true == m_bStressTest) ? "Stop Stress Test" : "Start Stress Test";
	if (ImGui::Button(szStressButon))
	{
		if (true == m_bStressTest)
		{
			m_bStressTest = false;
			//m_fStressRadian = 0.f;
			//m_vStressPosition = Vec3::Zero;
		}
		else
		{
			m_bStressTest = true;
		}
	}

	if (true == m_bStressTest)
	{
		if (FAILED(StressTest()))
		{
			ImGui::End();
			return E_FAIL;
		}
	}

	ImGui::End();

	return S_OK;
}

HRESULT CNavMeshView::LateTick()
{
	return S_OK;
}

HRESULT CNavMeshView::DebugRender()
{
	m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);
	
	m_pBatch->Begin();
	if (false == m_vecCells.empty())
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

			if (true == (*cell)->isNew)
			{
				DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::Blue);
			}
			else
			{
				DX::DrawTriangle(m_pBatch, vP0, vP1, vP2, Colors::LimeGreen);
			}

			++cell;
		}
	}

	if (false == m_vecObstacles.empty())
	{
		for (_int i = 0; i < m_vecObstacles.size(); ++i)
		{
			for (_int j = 0; j < m_vecObstacles[i]->vecPoints.size() - 1; ++j)
			{
				Vec3 vLine1 =
				{
					m_vecObstacles[i]->vecPoints[j].x,
					0.0f,
					m_vecObstacles[i]->vecPoints[j].z
				};
				Vec3 vLine2 =
				{
					m_vecObstacles[i]->vecPoints[j + 1].x,
					0.0f,
					m_vecObstacles[i]->vecPoints[j + 1].z,
				};

				m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
			}

			Vec3 vLine1 =
			{
				m_vecObstacles[i]->vecPoints[m_vecObstacles[i]->vecPoints.size() - 1].x,
				0.0f,
				m_vecObstacles[i]->vecPoints[m_vecObstacles[i]->vecPoints.size() - 1].z

			};
			Vec3 vLine2 =
			{
				m_vecObstacles[i]->vecPoints[0].x,
				0.0f,
				m_vecObstacles[i]->vecPoints[0].z
			};

			m_pBatch->DrawLine(VertexPositionColor(vLine1, Colors::Red), VertexPositionColor(vLine2, Colors::Red));
		}
	}

	if (false == m_vecObstaclePointSpheres.empty())
	{
		for (_int i = 0; i < m_vecObstaclePointSpheres.size(); ++i)
		{
			DX::Draw(m_pBatch, m_vecObstaclePointSpheres[i], Colors::Red);
		}
	}
	m_pBatch->End();

	if (nullptr != m_pAgent)
	{
		m_pAgent->DebugRender();
	}

	return S_OK;
}

void CNavMeshView::ClearNeighbors(vector<CellData*>& vecCells)
{
	for (auto cell : vecCells)
	{
		for (_int i = LINE_AB; i < LINE_END; ++i)
		{
			cell->pNeighbors[i] = nullptr;
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
				vecCells[sour]->pNeighbors[LINE_AB] = vecCells[dest];
			}
			else if (true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_B], vecCells[sour]->vPoints[POINT_C]))
			{
				vecCells[sour]->pNeighbors[LINE_BC] = vecCells[dest];
			}
			else if (true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_C], vecCells[sour]->vPoints[POINT_A]))
			{
				vecCells[sour]->pNeighbors[LINE_CA] = vecCells[dest];
			}
		}
	}
}

void CNavMeshView::SetUpCells2Grids(vector<CellData*>& vecCells, OUT unordered_multimap<_int, CellData*>& umapCellGrids, const _int iGridCX, const _int iGridCZ)
{
	BoundingBox tAABB;
	tAABB.Extents = Vec3(iGridCX * 0.5f, 512.0f, iGridCZ * 0.5f);

	for (auto pCell : vecCells)
	{
		for (_int iX = 0; iX < 1024 / iGridCX; ++iX)
		{
			for (_int iZ = 0; iZ < 1024 / iGridCZ; ++iZ)
			{
				tAABB.Center = Vec3((iGridCX - gWorldCX) * 0.5f + iGridCX * iX, 0.0f, (iGridCZ - gWorldCZ) * 0.5f + iGridCZ * iZ);

				if (true == tAABB.Intersects(pCell->vPoints[POINT_A], pCell->vPoints[POINT_B], pCell->vPoints[POINT_C]))
				{
					_int iKey = iZ * gGridX + iX;
					umapCellGrids.emplace(iKey, pCell);
				}
			}
		}
	}
}

void CNavMeshView::SetUpObsts2Grids(vector<Obst*>& vecObstacles, OUT unordered_multimap<_int, Obst*>& umapObstGrids, const _int iGridCX, const _int iGridCZ)
{
	BoundingBox tAABB;
	tAABB.Extents = Vec3(iGridCX * 0.5f, 10.0f, iGridCZ * 0.5f);

	for (auto pObst : vecObstacles)
	{
		for (_int iX = 0; iX < 1024 / iGridCX; ++iX)
		{
			for (_int iZ = 0; iZ < 1024 / iGridCZ; ++iZ)
			{
				tAABB.Center = Vec3((iGridCX - gWorldCX) * 0.5f + iGridCX * iX, 0.0f, (iGridCZ - gWorldCZ) * 0.5f + iGridCZ * iZ);

				if (true == tAABB.Intersects(pObst->tAABB))
				{
					_int iKey = iZ * gGridX + iX;
					umapObstGrids.emplace(iKey, pObst);
				}
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
	
	if (false == m_umapCellGrids.empty()) { m_umapCellGrids.clear(); }
	
	if (false == m_umapObstGrids.empty()) { m_umapObstGrids.clear(); }

	for (_int i = 0; i < m_tOut.numberoftriangles; ++i)
	{
		_int iIdx1 = m_tOut.trianglelist[i * 3 + POINT_A];
		_int iIdx2 = m_tOut.trianglelist[i * 3 + POINT_B];
		_int iIdx3 = m_tOut.trianglelist[i * 3 + POINT_C];

		_float y1 = 0.0f, y2 = 0.0f, y3 = 0.0f;

		auto [begin, end] = m_umapCellGrids.equal_range(m_tOut.pointlist[iIdx1 * 2]);
		
		for (auto point = begin; point != end;)
		{
			if( != point)
		}

		if (m_umapPointHeights.end() != height)
		{
			if (== height->first - height->second.first)
			{
				y1 = height->second.second;
			}
		}

		height = m_umapPointHeights.find(m_tOut.pointlist[iIdx2 * 2] + m_tOut.pointlist[iIdx2 * 2 + 1]);
		if (m_umapPointHeights.end() != height) { y2 = height->first - height->second.first; }

		height = m_umapPointHeights.find(m_tOut.pointlist[iIdx3 * 2] + m_tOut.pointlist[iIdx3 * 2 + 1]);
		if (m_umapPointHeights.end() != height) { y3 = height->first - height->second.first; }

		Vec3 vtx[POINT_END] =
		{
			{ m_tOut.pointlist[iIdx1 * 2], y1, m_tOut.pointlist[iIdx1 * 2 + 1] },
			{ m_tOut.pointlist[iIdx2 * 2], y2, m_tOut.pointlist[iIdx2 * 2 + 1] },
			{ m_tOut.pointlist[iIdx3 * 2], y3, m_tOut.pointlist[iIdx3 * 2 + 1] },
		};

		CellData* pCellData = new CellData;
		pCellData->vPoints[POINT_A] = vtx[POINT_A];
		pCellData->vPoints[POINT_B] = vtx[POINT_B];
		pCellData->vPoints[POINT_C] = vtx[POINT_C];
		pCellData->CW();

		m_vecCells.push_back(pCellData);
	}

	SetUpNeighbors(m_vecCells);

	for (auto cell : m_vecCells)
	{
		cell->SetUpData();
	}

	SetUpCells2Grids(m_vecCells, m_umapCellGrids);
	SetUpObsts2Grids(m_vecObstacles, m_umapObstGrids);

	return S_OK;
}

HRESULT CNavMeshView::BakeSingleObstacleData()
{
	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	map<LAYERTAG, class CLayer*>::iterator iter = mapLayers.find(LAYERTAG::GROUND);
	if (iter == mapLayers.end())
		return E_FAIL;

	vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();

	vector<Vec3> vecOutlines;

	vecObjects[0]->GetTransform()->SetPosition(Vec3::Zero);
	if (FAILED(CalculateObstacleOutline(vecObjects[0], vecOutlines)))
		return E_FAIL;

	Obst* pObst = new Obst;

	for (_int i = 0; i < vecOutlines.size(); ++i)
	{
		pObst->vecPoints.emplace_back(vecOutlines[i]);
	}

	TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
	for (auto vPoint : pObst->vecPoints)
	{
		if (fMaxX < vPoint.x) fMaxX = vPoint.x;
		if (fMinX > vPoint.x) fMinX = vPoint.x;

		if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
		if (fMinZ > vPoint.z) fMinZ = vPoint.z;
	}

	const _float fAABBOffset = 0.05f;
	pObst->tAABB.Center = Vec3((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
	pObst->tAABB.Extents = Vec3((fMaxX - fMinX) * 0.5f + fAABBOffset, 10.f, (fMaxZ - fMinZ) * 0.5f + fAABBOffset);

	SetPolygonHoleCenter(*pObst);

	m_vecObstacles.push_back(pObst);
	s2cPushBack(m_strObstacles, Utils::ToString(vecObjects[0]->GetObjectTag()));

	if (FAILED(DynamicCreate(*pObst)))
		return E_FAIL;

	for (auto& iter : pObst->vecPoints)
	{
		BoundingSphere tSphere(iter, 0.1f);

		m_vecObstaclePointSpheres.emplace_back(tSphere);
	}

	return S_OK;
}

HRESULT CNavMeshView::BakeHeightMapObstacles()
{
	Reset();
	InitialSetting();

	vector<vector<Vec3>> vecOutlines;

	if (FAILED(CalculateTerrainOutline(vecOutlines)))
		return E_FAIL;

	Obst* pObst = nullptr;

	for (_int i = 0; i < vecOutlines.size(); ++i)
	{
		pObst = new Obst;

		for (_int j = 0; j < vecOutlines[i].size(); ++j)
		{
			pObst->vecPoints.emplace_back(vecOutlines[i][j]);
		}

		TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
		for (auto vPoint : pObst->vecPoints)
		{
			if (fMaxX < vPoint.x) fMaxX = vPoint.x;
			if (fMinX > vPoint.x) fMinX = vPoint.x;

			if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
			if (fMinZ > vPoint.z) fMinZ = vPoint.z;
		}

		const _float fAABBOffset = 0.05f;
		pObst->tAABB.Center = Vec3((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
		pObst->tAABB.Extents = Vec3((fMaxX - fMinX) * 0.5f + fAABBOffset, 10.f, (fMaxZ - fMinZ) * 0.5f + fAABBOffset);

		SetPolygonHoleCenter(*pObst);

		m_vecObstacles.push_back(pObst);
		s2cPushBack(m_strObstacles, to_string(m_vecObstacles.size()));

		//DynamicCreate(*pObst);

		for (auto& iter : pObst->vecPoints)
		{
			BoundingSphere tSphere(iter, 0.1f);

			m_vecObstaclePointSpheres.emplace_back(tSphere);
		}
	}

	//

	for (auto pObst : m_vecObstacles)
	{
		for (auto& vPoint : pObst->vecPoints)
		{
			m_vecPoints.push_back(vPoint);
		}
	}

	SafeReleaseTriangle(m_tIn);
	SafeReleaseTriangle(m_tOut);

	UpdatePointList(m_tIn, m_vecPoints);
	UpdateSegmentList(m_tIn, m_vecPoints);
	UpdateHoleList(m_tIn);
	UpdateRegionList(m_tIn);

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);

	if (FAILED(BakeNavMesh()))
	{
		return E_FAIL;
	}
	//

	return S_OK;
}

HRESULT CNavMeshView::BakeHeightMap3D()
{
	Reset();
	//InitialSetting();
	
	m_vecPoints.push_back(Vec3(-512.0f, 0.f, -512.0f));
	m_vecPoints.push_back(Vec3(+512.0f, 0.f, -512.0f));
	m_vecPoints.push_back(Vec3(+512.0f, 0.f, +512.0f));
	m_vecPoints.push_back(Vec3(-512.0f, 0.f, +512.0f));

	m_iStaticPointCount = m_vecPoints.size();
	m_vecSegments.push_back(m_iStaticPointCount);

	if (FAILED(UpdatePointList(m_tIn, m_vecPoints)))
		return E_FAIL;

	if (FAILED(UpdateSegmentList(m_tIn, m_vecPoints)))
		return E_FAIL;

	if (FAILED(UpdateHoleList(m_tIn)))
		return E_FAIL;

	if (FAILED(UpdateRegionList(m_tIn)))
		return E_FAIL;
	
	//
	vector<vector<Vec3>> vecOutlines;

	if (FAILED(CalculateHillOutline(vecOutlines)))
	{
		return E_FAIL;
	}

	for (_int i = 0; i < vecOutlines.size(); ++i)
	{
		m_tIn.numberofpoints += vecOutlines[i].size();
	}

	SAFE_REALLOC(TRI_REAL, m_tIn.pointlist, m_tIn.numberofpoints * 2)

	_int k = m_iStaticPointCount;
	for (_int i = 0; i < vecOutlines.size(); ++i)
	{
		for (_int j = 0; j < vecOutlines[i].size(); ++j)
		{
			m_tIn.pointlist[2 * k + 0] = vecOutlines[i][j].x;
			m_tIn.pointlist[2 * k + 1] = vecOutlines[i][j].z;

			m_vecPoints.push_back(vecOutlines[i][j]);
			++k;
		}
		m_vecSegments.push_back(m_vecPoints.size());
	}

	m_tIn.numberofsegments = m_tIn.numberofpoints;
	if (0 < m_tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, m_tIn.segmentlist, m_tIn.numberofsegments * 2)

		_int iStartIndex = m_iStaticPointCount;
		for (_int i = 0; i < vecOutlines.size(); ++i)
		{
			for (_int j = 0; j < vecOutlines[i].size() - 1; ++j)
			//for (_int j = 0; j < m_vecSegments[i]; ++j)
			{
				m_tIn.segmentlist[2 * (iStartIndex + j) + 0] = iStartIndex + j + 0;
				m_tIn.segmentlist[2 * (iStartIndex + j) + 1] = iStartIndex + j + 1;
				++k;
			}
			_int iCache = iStartIndex + vecOutlines[i].size() - 1;
			m_tIn.segmentlist[2 * iCache + 0] = iCache;
			m_tIn.segmentlist[2 * iCache + 1] = iStartIndex;

			iStartIndex += vecOutlines[i].size();
		}
	}

	if (false == m_umapPointHeights.empty()) { m_umapPointHeights.clear(); }

	for (_int i = 0; i < m_vecPoints.size(); ++i)
	{
		m_umapPointHeights.emplace(m_vecPoints[i].x + m_vecPoints[i].z, pair(m_vecPoints[i].z, m_vecPoints[i].y));
	}

	UpdateHoleList(m_tIn);
	UpdateRegionList(m_tIn);

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);

	if (FAILED(BakeNavMesh()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdatePointList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst)
{
	tIn.numberofpoints = vecPoints.size() + ((nullptr == pObst) ? 0 : pObst->vecPoints.size());
	if (0 < m_tIn.numberofpoints)
	{
		SAFE_REALLOC(TRI_REAL, tIn.pointlist, tIn.numberofpoints * 2)

		_int i = 0;
		for (auto point : vecPoints)
		{
			tIn.pointlist[2 * i + 0] = point.x;
			tIn.pointlist[2 * i + 1] = point.z;
			++i;
		}

		// tObst
		if (nullptr != pObst)
		{
			for (_int j = 0; j < pObst->vecPoints.size(); ++j)
			{
				tIn.pointlist[2 * (i + j) + 0] = pObst->vecPoints[j].x;
				tIn.pointlist[2 * (i + j) + 1] = pObst->vecPoints[j].z;
			}
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateSegmentList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst)
{
	tIn.numberofsegments = vecPoints.size() + ((nullptr == pObst) ? 0 : pObst->vecPoints.size());
	if (0 < tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, tIn.segmentlist, tIn.numberofsegments * 2)

		// Points
		_int iStartIndex = ((nullptr == pObst) ? m_iStaticPointCount : vecPoints.size());
		for (_int i = 0; i < iStartIndex - 1; ++i)
		{
			tIn.segmentlist[2 * i + 0] = i + 0;
			tIn.segmentlist[2 * i + 1] = i + 1;
		}
		tIn.segmentlist[2 * (iStartIndex - 1) + 0] = iStartIndex - 1;
		tIn.segmentlist[2 * (iStartIndex - 1) + 1] = 0;

		// Obstacles
		_int iSegmentCount = ((nullptr == pObst) ? m_iStaticPointCount : iStartIndex);
		_int iRepeatCount = ((nullptr == pObst) ? m_vecObstacles.size() : 1);
		for (_int j = 0; j < iRepeatCount; ++j)
		{
			const Obst& tObst = ((nullptr == pObst) ? *m_vecObstacles[j] : *pObst);

			for (_int i = 0; i < tObst.vecPoints.size() - 1; ++i)
			{
				tIn.segmentlist[2 * (iSegmentCount + i) + 0] = iSegmentCount + i + 0;
				tIn.segmentlist[2 * (iSegmentCount + i) + 1] = iSegmentCount + i + 1;
			}
			_int iCache = iSegmentCount + tObst.vecPoints.size() - 1;
			tIn.segmentlist[2 * iCache + 0] = iCache;
			tIn.segmentlist[2 * iCache + 1] = iSegmentCount;

			if (nullptr == pObst)
			{
				iSegmentCount += tObst.vecPoints.size();
			}
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateHoleList(triangulateio& tIn, const Obst* pObst)
{
	tIn.numberofholes = ((nullptr == pObst) ? m_vecObstacles.size() : 1);
	if (0 < tIn.numberofholes)
	{
		SAFE_REALLOC(TRI_REAL, tIn.holelist, tIn.numberofholes * 2)

		for (_int i = 0; i < tIn.numberofholes; ++i)
		{
			const Obst& tObst = ((nullptr == pObst) ? *m_vecObstacles[i] : *pObst);

			tIn.holelist[2 * i + 0] = tObst.vInnerPoint.x;
			tIn.holelist[2 * i + 1] = tObst.vInnerPoint.z;
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateRegionList(triangulateio& tIn, const Obst* pObst)
{
	tIn.numberofregions = m_vecRegions.size();
	if (0 < tIn.numberofregions)
	{
		SAFE_REALLOC(TRI_REAL, tIn.regionlist, tIn.numberofregions * 4)

		for (_int i = 0; i < m_vecRegions.size(); ++i)
		{
			tIn.regionlist[4 * i + 0] = m_vecRegions[i].x;
			tIn.regionlist[4 * i + 1] = m_vecRegions[i].z;
			tIn.regionlist[4 * i + 1] = 0.f; //
			tIn.regionlist[4 * i + 1] = 0.f; //
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::StaticCreate(const Obst& tObst/**/)
{
	if (3 > m_vecPoints.size())
	{
		return E_FAIL;
	}

	SafeReleaseTriangle(m_tOut);

	UpdatePointList(m_tIn, m_vecPoints);
	UpdateSegmentList(m_tIn, m_vecPoints);
	UpdateHoleList(m_tIn);
	UpdateRegionList(m_tIn);

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);

	return S_OK;
}

HRESULT CNavMeshView::DynamicCreate(const Obst& tObst)
{
	set<CellData*> setIntersected;
	map<Vec3, pair<Vec3, CellData*>> mapOutlineCells;

	if (FAILED(GetIntersectedCells(tObst, setIntersected, true)))
	{
		return E_FAIL;
	}

	for (auto tCell : setIntersected)
	{
		for (uint8 i = 0; i < LINE_END; ++i)
		{	// neighbor가 유효한 edge 추출
			if (setIntersected.end() == setIntersected.find(tCell->pNeighbors[i]))
			{	// 해당 edge는 outline
				if (mapOutlineCells.end() != mapOutlineCells.find(tCell->vPoints[i]))
				{
					_int a = 0;
				}
				else
				{
					mapOutlineCells.emplace(tCell->vPoints[i], pair(tCell->vPoints[(i + 1) % POINT_END], tCell->pNeighbors[i]));
				}
			}
		}
	}

	vector<Vec3> vecOutlineCW;	// 시계 방향 정렬
	vecOutlineCW.emplace_back(mapOutlineCells.begin()->first);

	while (vecOutlineCW.size() < mapOutlineCells.size())
	{
		auto pair = mapOutlineCells.find(vecOutlineCW.back());
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

	if (FAILED(UpdatePointList(tIn, vecOutlineCW, &tObst)))
		return E_FAIL;

	if (FAILED(UpdateSegmentList(tIn, vecOutlineCW, &tObst)))
		return E_FAIL;

	if (FAILED(UpdateHoleList(tIn, &tObst)))
		return E_FAIL;

	if (FAILED(UpdateRegionList(tIn, &tObst)))
		return E_FAIL;

	triangulate(m_szTriswitches, &tIn, &tOut, nullptr);

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
		pCellData->vPoints[POINT_A] = vtx[POINT_A];
		pCellData->vPoints[POINT_B] = vtx[POINT_B];
		pCellData->vPoints[POINT_C] = vtx[POINT_C];
		pCellData->CW();
		pCellData->SetUpData();

		vecNewCells.emplace_back(pCellData);
	}

	SetUpNeighbors(vecNewCells);

	for (auto cell : vecNewCells)
	{	// 재구성된 데이터를 다시 전체 맵에 연결.
		for (uint8 i = LINE_AB; i < LINE_END; ++i)
		{
			if (nullptr == cell->pNeighbors[i])
			{	// new cell의 outline은 neighbor가 없음.
				auto OutCell = mapOutlineCells.find(cell->vPoints[i]);
				if (mapOutlineCells.end() != OutCell)
				{
					if (cell->vPoints[(i + 1) % POINT_END] == OutCell->second.first)
					{	// mapOutlineCells에서 outline 찾았다면 상호 연결
						cell->pNeighbors[i] = OutCell->second.second;

						if (nullptr == OutCell->second.second || OutCell->second.second == cell)
						{
							continue;
						}

						if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_A], OutCell->second.second->vPoints[POINT_B]))
						{
							OutCell->second.second->pNeighbors[LINE_AB] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_B], OutCell->second.second->vPoints[POINT_C]))
						{
							OutCell->second.second->pNeighbors[LINE_BC] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_C], OutCell->second.second->vPoints[POINT_A]))
						{
							OutCell->second.second->pNeighbors[LINE_CA] = cell;
						}
					}
				}
			}
		}

		cell->isNew = true;
		m_vecCells.emplace_back(cell);
	}

	for (auto cell : vecNewCells)
	{
		cell->SetUpData();
	}

	SafeReleaseTriangle(tIn);
	SafeReleaseTriangle(tOut);

	return S_OK;
}

HRESULT CNavMeshView::DynamicCreate(CGameObject* const pGameObject)
{
	auto ObstPrefab = m_mapObstaclePrefabs.find(pGameObject->GetObjectTag());
	if (m_mapObstaclePrefabs.end() != ObstPrefab)
	{
		Obst* pObst = new Obst(ObstPrefab->second, pGameObject->GetTransform()->WorldMatrix());
		if (FAILED(DynamicCreate(*pObst)))
		{
			return E_FAIL;
		}

		m_hmapObstacleIndex[pGameObject] = m_vecObstacles.size();	// 불편하지만 트랜스촘 변환을 위해... 일단은 이렇게 해두자.
		m_vecObstacles.push_back(pObst);
	}

	return S_OK;
}

HRESULT CNavMeshView::UpdateObstacleTransform(CGameObject* const pGameObject)
{
	auto Obst = m_hmapObstacleIndex.find(pGameObject);

	if (m_hmapObstacleIndex.end() == Obst)
	{
		return E_FAIL;
	}

	if (FAILED(DynamicDelete(*m_vecObstacles[Obst->second])))
	{
		return E_FAIL;
	}

	Safe_Delete(m_vecObstacles[Obst->second]);
	//Safe_Delete(m_strObstacles[Obst->second]);

	auto iter = m_vecObstacles.begin() + Obst->second;
	//auto iterStr = m_strObstacles.begin() + Obst->second;
	m_vecObstacles.erase(iter);
	//m_strObstacles.erase(iterStr);

	if (FAILED(DynamicCreate(pGameObject)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNavMeshView::DynamicDelete(const Obst& tObst)
{
	set<CellData*> setIntersected;
	map<Vec3, pair<Vec3, CellData*>> mapOutlineCells;

	if (FAILED(GetIntersectedCells(tObst, setIntersected)))
	{
		return E_FAIL;
	}

	for (auto tCell : setIntersected)
	{
		for (uint8 i = 0; i < LINE_END; ++i)
		{	// neighbor가 유효한 edge 추출
			if (setIntersected.end() == setIntersected.find(tCell->pNeighbors[i]))
			{	// 해당 edge는 outline
				if (mapOutlineCells.end() != mapOutlineCells.find(tCell->vPoints[i]))
				{
					_int a = 0;
				}
				else
				{
					mapOutlineCells.emplace(tCell->vPoints[i], pair(tCell->vPoints[(i + 1) % POINT_END], tCell->pNeighbors[i]));
				}
			}
		}
	}

	vector<Vec3> vecOutlineCW;

	vecOutlineCW.emplace_back(mapOutlineCells.begin()->first);

	for(_int i = 0; i < mapOutlineCells.size() - tObst.vecPoints.size() - 1; ++i)
	{
		auto pair = mapOutlineCells.find(vecOutlineCW.back());

		if (mapOutlineCells.end() != pair)
		{
			vecOutlineCW.emplace_back(pair->second.first);
		}
	}

	for (auto tCell : setIntersected)
	{
		tCell->isDead = true;
	}

	triangulateio tIn = { 0 }, tOut = { 0 };

	if (FAILED(UpdatePointList(tIn, vecOutlineCW)))
	{
		return E_FAIL;
	}

#pragma region segmentlist
	tIn.numberofsegments = vecOutlineCW.size();
	if (0 < tIn.numberofsegments)
	{
		SAFE_REALLOC(_int, tIn.segmentlist, tIn.numberofsegments * 2)

		// Points
		for (_int i = 0; i < tIn.numberofsegments - 1; ++i)
		{
			tIn.segmentlist[2 * i + 0] = i + 0;
			tIn.segmentlist[2 * i + 1] = i + 1;
		}
		tIn.segmentlist[2 * (vecOutlineCW.size() - 1) + 0] = vecOutlineCW.size() - 1;
		tIn.segmentlist[2 * (vecOutlineCW.size() - 1) + 1] = 0;
	}
#pragma endregion segmentlist

#pragma region holelist
	tIn.numberofholes = 0;
#pragma endregion holelist

#pragma region regionlist
	tIn.numberofregions = 0;
#pragma endregion regionlist

	triangulate(m_szTriswitches, &tIn, &tOut, nullptr);

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
		pCellData->vPoints[POINT_A] = vtx[POINT_A];
		pCellData->vPoints[POINT_B] = vtx[POINT_B];
		pCellData->vPoints[POINT_C] = vtx[POINT_C];
		pCellData->CW();
		//pCellData->SetUpData();

		vecNewCells.emplace_back(pCellData);
	}

	SetUpNeighbors(vecNewCells);

	for (auto cell : vecNewCells)
	{	// 재구성된 데이터를 다시 전체 맵에 연결.
		for (uint8 i = LINE_AB; i < LINE_END; ++i)
		{
			if (nullptr == cell->pNeighbors[i])
			{	// new cell의 outline은 neighbor가 없음.
				auto OutCell = mapOutlineCells.find(cell->vPoints[i]);
				if (mapOutlineCells.end() != OutCell)
				{
					if (cell->vPoints[(i + 1) % POINT_END] == OutCell->second.first)
					{	// mapOutlineCells에서 outline 찾았다면 상호 연결
						cell->pNeighbors[i] = OutCell->second.second;

						if (nullptr == OutCell->second.second || OutCell->second.second == cell)
						{
							continue;
						}

						if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_A], OutCell->second.second->vPoints[POINT_B]))
						{
							OutCell->second.second->pNeighbors[LINE_AB] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_B], OutCell->second.second->vPoints[POINT_C]))
						{
							OutCell->second.second->pNeighbors[LINE_BC] = cell;
						}
						else if (true == cell->ComparePoints(OutCell->second.second->vPoints[POINT_C], OutCell->second.second->vPoints[POINT_A]))
						{
							OutCell->second.second->pNeighbors[LINE_CA] = cell;
						}
					}
				}
			}
		}

		cell->isNew = true;
		m_vecCells.emplace_back(cell);
	}

	for (auto cell : vecNewCells)
	{
		cell->SetUpData();
	}

	SafeReleaseTriangle(tIn);
	SafeReleaseTriangle(tOut);

	return S_OK;
}

HRESULT CNavMeshView::CreateAgent(Vec3 vSpawnPosition)
{
	CellData* pCell = FindCellByPosition(vSpawnPosition);

	if (nullptr == pCell)
	{
		return E_FAIL;
	}

	CAgent::AgentDesc tDesc =
	{
		pCell,
		&m_vecCells,
		&m_umapCellGrids,
		&m_umapObstGrids
	};

	m_pAgent = static_cast<CAgent*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Agent"), LAYERTAG::PLAYER, &tDesc));

	if (nullptr == m_pAgent)
	{
		return E_FAIL;
	}

	m_pAgent->GetTransform()->SetPosition(vSpawnPosition);

	return S_OK;
}

HRESULT CNavMeshView::CreateAgent(_int iSpawnIndex)
{
	Vec3 vSpawnPosition = Vec3::Zero;

	for (auto& point : m_vecCells[iSpawnIndex]->vPoints)
	{
		vSpawnPosition += point;
	}

	vSpawnPosition /= 3.f;

	CAgent::AgentDesc tDesc =
	{
		m_vecCells[iSpawnIndex],
		&m_vecCells,
		&m_umapCellGrids,
		&m_umapObstGrids,
	};

	m_pAgent = static_cast<CAgent*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Agent"), LAYERTAG::PLAYER, &tDesc));
	
	if (nullptr == m_pAgent)
	{
		return E_FAIL;
	}

	m_pAgent->GetTransform()->SetPosition(vSpawnPosition);

	return S_OK;
}

HRESULT CNavMeshView::StressTest()
{
	if (nullptr != m_pStressObst)
	{
		if (FAILED(DynamicDelete(*m_pStressObst)))
		{
			return E_FAIL;
		}

		Safe_Delete(m_pStressObst);
	}

	if (nullptr == m_pStressObst)
	{
		//m_matStressOffset = XMMatrixRotationY(fTimeDelta);

		static _float fStressRadian = 0;
		static Vec3 vStressPosition = Vec3::Zero;
		if (fStressRadian > XM_PI)
		{
			fStressRadian -= XM_PI;
		}
		else
		{
			fStressRadian += 0.03f;
		}

		static const _float fSpeed = 0.03f;

		if (KEY_PRESSING(KEY::LEFT_ARROW))
			vStressPosition.x -= fSpeed;
		if (KEY_PRESSING(KEY::RIGHT_ARROW))
			vStressPosition.x += fSpeed;
		if (KEY_PRESSING(KEY::UP_ARROW))
			vStressPosition.z += fSpeed;
		if (KEY_PRESSING(KEY::DOWN_ARROW))
			vStressPosition.z -= fSpeed;

		Matrix matOffset = XMMatrixRotationY(fStressRadian) * XMMatrixTranslationFromVector(vStressPosition);

		m_pStressObst = new Obst;

		m_pStressObst->vecPoints.emplace_back(Vec3::Transform(Vec3(-10.0f, 0.0f, -10.0f), matOffset));
		m_pStressObst->vecPoints.emplace_back(Vec3::Transform(Vec3(-10.0f, 0.0f, +10.0f), matOffset));
		m_pStressObst->vecPoints.emplace_back(Vec3::Transform(Vec3(+10.0f, 0.0f, +10.0f), matOffset));
		m_pStressObst->vecPoints.emplace_back(Vec3::Transform(Vec3(+10.0f, 0.0f, -10.0f), matOffset));

		TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
		for (auto vPoint : m_pStressObst->vecPoints)
		{
			if (fMaxX < vPoint.x) fMaxX = vPoint.x;
			if (fMinX > vPoint.x) fMinX = vPoint.x;

			if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
			if (fMinZ > vPoint.z) fMinZ = vPoint.z;
		}

		SetPolygonHoleCenter(*m_pStressObst);

		const _float fAABBOffset = 0.05f;
		m_pStressObst->tAABB.Center = Vec3((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
		m_pStressObst->tAABB.Extents = Vec3((fMaxX - fMinX) * 0.5f + fAABBOffset, 10.f, (fMaxZ - fMinZ) * 0.5f + fAABBOffset);

		if (FAILED(DynamicCreate(*m_pStressObst)))
		{
			return E_FAIL;
		}
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
		if (false == tObst.IsOut(vCenter))
		{
			tObst.vInnerPoint = vCenter;
			break;
		}
	}
}

HRESULT CNavMeshView::GetIntersectedCells(const Obst& tObst, OUT set<CellData*>& setIntersected, _bool bPop)
{
#pragma region CPU
	/*for (_int i = 0; i < m_vecCells.size(); ++i)
	{
		if (true == m_vecCells[i]->isDead)
		{
			auto iter = m_vecCells.begin() + i;
			m_vecCells.erase(iter);
			--i;

			continue;
		}

		if (true == m_vecCells[i]->isNew)
		{
			m_vecCells[i]->isNew = false;
		}

		if (false == m_vecCells[i]->isDead && true == tObst.tAABB.Intersects(m_vecCells[i]->vPoints[POINT_A], m_vecCells[i]->vPoints[POINT_B], m_vecCells[i]->vPoints[POINT_C]))
		{
			setIntersected.emplace(m_vecCells[i]);
		}
	}

	return S_OK;*/

	Vec3 vLB = tObst.tAABB.Center - tObst.tAABB.Extents;
	Vec3 vRT = tObst.tAABB.Center + tObst.tAABB.Extents;

	_int iLB_X = (vLB.x + gWorldCX * 0.5f) / gGridCX;
	_int iLB_Z = (vLB.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iRT_X = (vRT.x + gWorldCX * 0.5f) / gGridCX;
	_int iRT_Z = (vRT.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iLB_Key = iLB_Z * gGridX + iLB_X;
	_int iRT_Key = iRT_Z * gGridX + iRT_X;

	for (_int iKeyZ = iLB_Z; iKeyZ <= iRT_Z; ++iKeyZ)
	{
		for (_int iKeyX = iLB_X; iKeyX <= iRT_X; ++iKeyX)
		{
			_int iKey = iKeyZ * gGridX + iKeyX;

			auto [begin, end] = m_umapCellGrids.equal_range(iKey);

			for (auto cell = begin; cell != end;)
			{
				if (true == cell->second->isNew)
				{
					cell->second->isNew = false;
				}

				if (false == cell->second->isDead && true == tObst.tAABB.Intersects(cell->second->vPoints[POINT_A], cell->second->vPoints[POINT_B], cell->second->vPoints[POINT_C]))
				{
					setIntersected.emplace(cell->second);

					if (true == bPop)
					{
						cell = m_umapCellGrids.erase(cell);
						continue;
					}
				}

				++cell;
			}
		}
	}

	return S_OK;
#pragma endregion CPU

#pragma region GPU
	//vector<array<Vec3, 3>> vecTriangle(m_vecCells.size());
	//CStructuredBuffer* pStructuredBuffer = nullptr;

	//for (_int i = 0; i < m_vecCells.size(); ++i)
	//{
	//	if (true == m_vecCells[i]->isDead)
	//	{
	//		auto iter = m_vecCells.begin() + i;
	//		m_vecCells.erase(iter);
	//		--i;

	//		continue;
	//	}

	//	if (true == m_vecCells[i]->isNew)
	//	{
	//		m_vecCells[i]->isNew = false;
	//	}

	//	vecTriangle[i] = m_vecCells[i]->vPoints;	// CellData를 경량화 해서 복사 없이 바로 전달하도록 만들어보자.
	//}

	//vecTriangle.shrink_to_fit();
	//vector<BOOL> vecTriTestResult(m_vecCells.size());

	//pStructuredBuffer =
	//	CStructuredBuffer::Create(
	//	m_pDevice,
	//	m_pContext,
	//	vecTriangle.data(),
	//	3 * sizeof(Vec3),
	//	m_vecCells.size(),
	//	sizeof(BOOL),
	//	m_vecCells.size()
	//	);

	//if (FAILED(m_pCS_TriTest->Bind_RawValue("gObstCenter", &tObst.tAABB.Center, sizeof(Vec3))) || 
	//	FAILED(m_pCS_TriTest->Bind_RawValue("gObstExtents", &tObst.tAABB.Extents, sizeof(Vec3))))
	//{
	//	return E_FAIL;
	//}
	//
	//if (FAILED(m_pCS_TriTest->Bind_Resource("InputCell", pStructuredBuffer->GetSRV())))
	//{
	//	return E_FAIL;
	//}
	//
	//if (FAILED(m_pCS_TriTest->Get_UAV("Output", pStructuredBuffer->GetUAV())))
	//{
	//	return E_FAIL;
	//}

	//if(FAILED(m_pCS_TriTest->Dispatch(0, ceil(m_vecCells.size() / 128.0), 1, 1)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(pStructuredBuffer->CopyFromOutput(vecTriTestResult.data())))
	//{
	//	return E_FAIL;
	//}

	//for (_int i = 0; i < vecTriTestResult.size(); ++i)
	//{
	//	if (false != vecTriTestResult[i])
	//	{
	//		setIntersected.emplace(m_vecCells[i]);
	//	}
	//}

	//Safe_Release(pStructuredBuffer);
#pragma endregion GPU
}

HRESULT CNavMeshView::CalculateObstacleOutline(CGameObject* const pGameObject, OUT vector<Vec3>& vecOutline)
{
	CModel* const pModel = pGameObject->GetModel();

	if (nullptr == pModel)
	{
		return E_FAIL;
	}

	const vector<Vec3>& vecSurfaceVtx = pModel->GetSurfaceVtx();
	const vector<FACEINDICES32>& vecSurfaceIdx = pModel->GetSurfaceIdx();

	_float fDistance = FLT_MAX;
	_float fMinDistance = FLT_MAX;
	Vec3   vPickPosition = -Vec3::One;

	Ray cVerticalRay;
	Ray cHorizontalRay;

	vector<iVec3> vecIntersected;

	//for (_int i = -512; i < 512; ++i)
	for (_int i = -64; i < 64; ++i)
	{
		cVerticalRay.position = Vec3((_float)i, 0.05f, -64.0f);
		cVerticalRay.direction = Vec3::Backward;

		cHorizontalRay.position = Vec3(-64.0f, 0.05f, (_float)i);
		cHorizontalRay.direction = Vec3::Right;

		for (_int j = 0; j < vecSurfaceIdx.size(); ++j)
		{
			if (cVerticalRay.Intersects(
				vecSurfaceVtx[vecSurfaceIdx[j]._0],
				vecSurfaceVtx[vecSurfaceIdx[j]._1],
				vecSurfaceVtx[vecSurfaceIdx[j]._2],
				OUT fDistance))
			{
				Vec3 vPos = cVerticalRay.position + cVerticalRay.direction * fDistance;

				if (isnan(vPos.x) || isnan(vPos.y) || isnan(vPos.z) || isnan(fDistance))
				{
					continue;
				}

				vecIntersected.emplace_back(iVec3(round(vPos.x), round(vPos.y), round(vPos.z)));
			}

			if (cHorizontalRay.Intersects(
				vecSurfaceVtx[vecSurfaceIdx[j]._0],
				vecSurfaceVtx[vecSurfaceIdx[j]._1],
				vecSurfaceVtx[vecSurfaceIdx[j]._2],
				OUT fDistance))
			{
				Vec3 vPos = cHorizontalRay.position + cHorizontalRay.direction * fDistance;

				if (isnan(vPos.x) || isnan(vPos.y) || isnan(vPos.z) || isnan(fDistance))
				{
					continue;
				}

				vecIntersected.emplace_back(iVec3(round(vPos.x), round(vPos.y), round(vPos.z)));
			}
		}
	}

	iVec3 vStart = *vecIntersected.begin();

	set<iVec3> setPoints(vecIntersected.begin(), vecIntersected.end());
	vector<Vec3> vecExpandedOutline;
	vector<iVec3> vecTightOutline;
	vector<Vec3> vecClearOutline;

	Dfs(vStart, setPoints, vecTightOutline);

	vecExpandedOutline = ExpandOutline(vecTightOutline, 2.0f);
	vecClearOutline = ProcessIntersections(vecExpandedOutline);
	
	RamerDouglasPeucker(vecClearOutline, 1.0f, vecOutline);

	return S_OK;
}

HRESULT CNavMeshView::CalculateTerrainOutline(OUT vector<vector<Vec3>>& vecOutlines)
{
	if (nullptr == m_pTerrainBuffer)
	{
		return E_FAIL;
	}

	const vector<Vec3>& vecSurfaceVtx = m_pTerrainBuffer->GetTerrainVertices();
	const vector<FACEINDICES32>& vecSurfaceIdx = m_pTerrainBuffer->GetTerrainIndices();

	_float fDistance = FLT_MAX;
	_float fMinDistance = FLT_MAX;
	Vec3   vPickPosition = -Vec3::One;

	Ray cVerticalRay;
	Ray cHorizontalRay;

	vector<vector<_int>> vecIntersected(1024, vector<_int>(1024, 0));

	for (_int i = -512; i < 512; ++i)
	{
		cVerticalRay.position = Vec3((_float)i, 0.2f, -512.0f);
		cVerticalRay.direction = Vec3::Backward;

		cHorizontalRay.position = Vec3(-512.0f, 0.2f, (_float)i);
		cHorizontalRay.direction = Vec3::Right;

		for (_int j = 0; j < vecSurfaceIdx.size(); ++j)
		{
			if (cVerticalRay.Intersects(
				vecSurfaceVtx[vecSurfaceIdx[j]._0],
				vecSurfaceVtx[vecSurfaceIdx[j]._1],
				vecSurfaceVtx[vecSurfaceIdx[j]._2],
				OUT fDistance))
			{
				Vec3 vPos = cVerticalRay.position + cVerticalRay.direction * fDistance;

				if (isnan(vPos.x) || isnan(vPos.y) || isnan(vPos.z) || isnan(fDistance))
				{
					continue;
				}

				vecIntersected[round(vPos.x) + 512][round(vPos.z) + 512] = true;
			}

			if (cHorizontalRay.Intersects(
				vecSurfaceVtx[vecSurfaceIdx[j]._0],
				vecSurfaceVtx[vecSurfaceIdx[j]._1],
				vecSurfaceVtx[vecSurfaceIdx[j]._2],
				OUT fDistance))
			{
				Vec3 vPos = cHorizontalRay.position + cHorizontalRay.direction * fDistance;

				if (isnan(vPos.x) || isnan(vPos.y) || isnan(vPos.z) || isnan(fDistance))
				{
					continue;
				}

				vecIntersected[round(vPos.x) + 512][round(vPos.z) + 512] = true;
			}
		}
	}

	vector<vector<Vec3>> vecExpandedOutlines;
	vector<vector<iVec3>> vecTightOutlines;
	vector<vector<Vec3>> vecClearOutlines;

	DfsTerrain(vecIntersected, vecTightOutlines);

	for (_int i = 0; i < vecTightOutlines.size(); ++i)
	{
		_float fDistance = 0.9f;
		vecExpandedOutlines.push_back(ExpandOutline(vecTightOutlines[i], fDistance));
	}
	
	for (_int i = 0; i < vecExpandedOutlines.size(); ++i)
	{
		vecClearOutlines.push_back(ProcessIntersections(vecExpandedOutlines[i]));
	}

	vecOutlines.resize(vecClearOutlines.size());
	for (_int i = 0; i < vecClearOutlines.size(); ++i)
	{
		_float fEpsilon = 1.2f;
		RamerDouglasPeucker(vecClearOutlines[i], fEpsilon, vecOutlines[i]);
	}	

	return S_OK;
}

HRESULT CNavMeshView::CalculateHillOutline(OUT vector<vector<Vec3>>& vecOutlines)
{
	if (nullptr == m_pTerrainBuffer)
	{
		return E_FAIL;
	}

	const vector<Vec3>& vecSurfaceVtx = m_pTerrainBuffer->GetTerrainVertices();
	const vector<FACEINDICES32>& vecSurfaceIdx = m_pTerrainBuffer->GetTerrainIndices();

	_float fDistance = FLT_MAX;
	_float fMinDistance = FLT_MAX;
	Vec3   vPickPosition = -Vec3::One;

	Ray cVerticalRay;
	Ray cHorizontalRay;

	vector<vector<_int>> vecIntersected(1024, vector<_int>(1024, 0));

	// 등고선을 terrain vtx y값으로 정하게 되면 맵 테두리 등에서 따로 처리할 문제가 생김.
	// 오래걸리더라도 0.05f 정도 작은 값 y에 offset주고 RayCasting해서 Outline 따내도록 해보자.
	// 이후 static 정점 & index 추가해서 삼각형 제대로 구성하도록 해보고 navmesh y적용해서 렌더링 및 지형타기까지.
	// 

	for (_int h = 0; h < 50; h += 2)
	{
		//for (_int i = -512; i < 512; ++i)
		for (_int i = -512; i < 512; i += 2)
		{
			cVerticalRay.position = Vec3((_float)i, (_float)h + 0.2f, -512.0f);
			cVerticalRay.direction = Vec3::Backward;

			cHorizontalRay.position = Vec3(-512.0f, (_float)h + 0.2f, (_float)i);
			cHorizontalRay.direction = Vec3::Right;

			for (_int j = 0; j < vecSurfaceIdx.size(); ++j)
			{
				if (cVerticalRay.Intersects(
					vecSurfaceVtx[vecSurfaceIdx[j]._0],
					vecSurfaceVtx[vecSurfaceIdx[j]._1],
					vecSurfaceVtx[vecSurfaceIdx[j]._2],
					OUT fDistance))
				{
					if (isnan(fDistance))
					{
						continue;
					}

					Vec3 vPos = cVerticalRay.position + cVerticalRay.direction * fDistance;

					_int iX = (_int)floor(vPos.x);
					_int iZ = (_int)floor(vPos.z);
					if (iX % 2 != 0)	++iX;
					if (iZ % 2 != 0)	++iZ;

					//vecIntersected[round(vPos.x) + 512][round(vPos.z) + 512] = (_int)round(vPos.y);
					vecIntersected[iX + 512][iZ + 512] = (_int)round(vPos.y);
				}

				if (cHorizontalRay.Intersects(
					vecSurfaceVtx[vecSurfaceIdx[j]._0],
					vecSurfaceVtx[vecSurfaceIdx[j]._1],
					vecSurfaceVtx[vecSurfaceIdx[j]._2],
					OUT fDistance))
				{
					if (isnan(fDistance))
					{
						continue;
					}

					Vec3 vPos = cHorizontalRay.position + cHorizontalRay.direction * fDistance;

					_int iX = (_int)floor(vPos.x);
					_int iZ = (_int)floor(vPos.z);
					if (iX % 2 != 0)	++iX;
					if (iZ % 2 != 0)	++iZ;

					//vecIntersected[round(vPos.x) + 512][round(vPos.z) + 512] = (_int)round(vPos.y);
					vecIntersected[iX + 512][iZ + 512] = (_int)round(vPos.y);
				}
			}
		}
	}

	vector<vector<Vec3>> vecExpandedOutlines;
	vector<vector<iVec3>> vecTightOutlines;
	vector<vector<Vec3>> vecClearOutlines;

	DfsTerrain(vecIntersected, vecTightOutlines);

	for (_int i = 0; i < vecTightOutlines.size(); ++i)
	{
		_float fDistance = 0.2f;
		vecExpandedOutlines.push_back(ExpandOutline(vecTightOutlines[i], fDistance));
	}

	for (_int i = 0; i < vecExpandedOutlines.size(); ++i)
	{
		vecClearOutlines.push_back(ProcessIntersections(vecExpandedOutlines[i]));
	}

	vecOutlines.resize(vecClearOutlines.size());
	for (_int i = 0; i < vecClearOutlines.size(); ++i)
	{
		_float fEpsilon = 0.5f;
		RamerDouglasPeucker(vecClearOutlines[i], fEpsilon, vecOutlines[i]);
	}

	//for (_int i = 0; i < vecOutlines.size(); ++i)
	for (auto iter = vecOutlines.begin(); iter != vecOutlines.end();)
	{
		if (iter->size() < 3)
		{
			iter->clear();
			iter = vecOutlines.erase(iter);
			continue;
		}
		++iter;
	}

	//for (_int i = 0; i < vecOutlines.size(); ++i)
	//{
	//	for (_int j = 0; j < vecOutlines[i].size(); ++j)
	//	{
	//		vecOutlines[i][j].y = /*0.4f * */vecOutlines[i][j].y + 0.1f;
	//	}
	//}

	return S_OK;
}

void CNavMeshView::Dfs(const iVec3& vStart, const set<iVec3>& setPoints, OUT vector<iVec3>& vecLongest)
{
	const vector<pair<_int, _int>> vecDirections =
	{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
	};

	stack<pair<iVec3, vector<iVec3>>> stkPoint;
	set<iVec3> setVisited;
	stkPoint.push({ vStart, {vStart} });

	while (!stkPoint.empty())
	{
		auto [vCurrent, vecPath] = stkPoint.top();
		stkPoint.pop();

		if (vecPath.size() > vecLongest.size())
		{
			vecLongest = vecPath;
		}

		for (const auto& vDir : vecDirections)
		{
			iVec3 vNeighbor(vCurrent.x + vDir.first, 0, vCurrent.z + vDir.second);
			
			if (setPoints.find(vNeighbor) != setPoints.end() &&
				setVisited.find(vNeighbor) == setVisited.end())
			{
				setVisited.emplace(vNeighbor);
				vector<iVec3> vecNewPath = vecPath;
				vecNewPath.push_back(vNeighbor);
				stkPoint.push({ vNeighbor, vecNewPath });
			}
		}
	}
}

void CNavMeshView::DfsTerrain(vector<vector<_int>>& vecPoints, OUT vector<vector<iVec3>>& vecOutlines)
{
	const vector<pair<_int, _int>> vecDirections =
	{
		//{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
		{2, 0}, {-2, 0}, {0, 2}, {0, -2}, {2, 2}, {-2, -2}, {2, -2}, {-2, 2}
	};

	_int iRows = vecPoints.size();
	_int iCols = vecPoints[0].size();
	set<iVec3> setVisited;

	for (_int i = 0; i < iRows; i += 2)
	{
		for (_int j = 0; j < iCols; j += 2)
		{
			if (vecPoints[i][j] && setVisited.find({ i - 512, vecPoints[i][j], j - 512 }) == setVisited.end())
			{
				stack<pair<iVec3, vector<iVec3>>> stkPoint;
				vector<iVec3> vecOutline;
				stkPoint.push({ {i - 512, vecPoints[i][j], j - 512}, {{i - 512, vecPoints[i][j], j - 512}} });
				setVisited.emplace(i - 512, vecPoints[i][j], j - 512);

				while (!stkPoint.empty())
				{
					auto [vCurrent, vecPath] = stkPoint.top();
					stkPoint.pop();

					for (const auto& vDir : vecDirections)
					{
						iVec3 vNeighbor(vCurrent.x + vDir.first, vCurrent.y, vCurrent.z + vDir.second);

						if (vNeighbor.x >= -512 && vNeighbor.x < iRows - 512 &&
							vNeighbor.z >= -512 && vNeighbor.z < iCols - 512 &&
							vecPoints[vNeighbor.x + 512][vNeighbor.z + 512] &&
							setVisited.find(vNeighbor) == setVisited.end())
						{
							setVisited.emplace(vNeighbor);
							vector<iVec3> vecNewPath = vecPath;
							vecNewPath.push_back(vNeighbor);
							stkPoint.push({ vNeighbor, vecNewPath });
						}
					}

					if (vecPath.size() > vecOutline.size())
					{
						vecOutline = vecPath;
					}
				}

				if (!vecOutline.empty())
				{
					vecOutlines.push_back(vecOutline);
				}
			}
		}
	}
}

Vec3 CNavMeshView::CalculateNormal(const iVec3& vPrev, const iVec3& vCurrent, const iVec3& vNext)
{
	Vec3 vDir1 = { (_float)(vCurrent.x - vPrev.x), 0, (_float)(vCurrent.z - vPrev.z) };
	Vec3 vDir2 = { (_float)(vNext.x - vCurrent.x), 0, (_float)(vNext.z - vCurrent.z) };

	vDir1.Normalize();
	vDir2.Normalize();

	Vec3 vNormal = { -vDir1.z - vDir2.z, 0.0f, vDir1.x + vDir2.x };
	
	vNormal.Normalize();

	return vNormal;
}

_bool CNavMeshView::IsClockwise(const vector<iVec3>& vecPoints)
{
	_float fSum = 0;

	for (_int i = 0; i < vecPoints.size(); ++i)
	{
		const iVec3& p1 = vecPoints[i];
		const iVec3& p2 = vecPoints[(i + 1) % vecPoints.size()];
		fSum += (p2.x - p1.x) * (p2.z + p1.z);
	}

	//return fSum < 0;
	return fSum > 0;
}

vector<Vec3> CNavMeshView::ExpandOutline(const vector<iVec3>& vecOutline, _float fDistance)
{
	vector<Vec3> vecExpandedOutline;
	_int iSize = vecOutline.size();

	_bool isClockwise = IsClockwise(vecOutline);

	for (_int i = 0; i < iSize; ++i)
	{
		const iVec3& vPrev = vecOutline[(i - 1 + iSize) % iSize];
		const iVec3& vCurrent = vecOutline[i];
		const iVec3& vNext = vecOutline[(i + 1) % iSize];
		Vec3 vNormal = CalculateNormal(vPrev, vCurrent, vNext);

		if (false == isClockwise)
		{
			vNormal = { -vNormal.x, 0.0f, -vNormal.z };
		}

		Vec3 vExpandedPoint = { (_float)vCurrent.x + vNormal.x * fDistance, (_float)(vCurrent.y), (_float)vCurrent.z + vNormal.z * fDistance };
		vecExpandedOutline.push_back(vExpandedPoint);
	}

	return vecExpandedOutline;
}

_bool CNavMeshView::IntersectSegments(const Vec3& vP1, const Vec3& vQ1, const Vec3& vP2, const Vec3& vQ2, Vec3& vIntersection)
{
	Vec3 vSour = { vQ1.x - vP1.x, 0.0f, vQ1.z - vP1.z };
	Vec3 vDest = { vQ2.x - vP2.x, 0.0f, vQ2.z - vP2.z };

	_float fSxD = vSour.x * vDest.z - vSour.z * vDest.x;
	_float fPQxR = (vP2.x - vP1.x) * vSour.z - (vP2.z - vP1.z) * vSour.x;

	//if (fabs(fSxD) < 1e-5f && fabs(fPQxR) < 1e-5f)
	//{
	//	return false; // Collinear
	//}
	if (fabs(fSxD) < 1e-5f)
	{
		return false; // Parallel
	}

	_float fT = ((vP2.x - vP1.x) * vDest.z - (vP2.z - vP1.z) * vDest.x) / fSxD;
	_float fU = fPQxR / fSxD;

	if (fT >= 0.0f && fT <= 1.0f && fU >= 0.0f && fU <= 1.0f)
	{
		vIntersection = { vP1.x + fT * vSour.x, 0.0f, vP1.z + fT * vSour.z };
		return true;
	}

	return false;
}

vector<Vec3> CNavMeshView::ProcessIntersections(vector<Vec3>& vecExpandedOutline)
{
	vector<Vec3> vecResult;
	_int iSize = vecExpandedOutline.size();

	for (_int i = 0; i < iSize; ++i)
	{
		const Vec3& vP1 = vecExpandedOutline[i];
		const Vec3& vQ1 = vecExpandedOutline[(i + 1) % iSize];

		_bool isIntersected = false;
		for (_int j = i + 2; j < iSize - 1; ++j)
		{
			const Vec3& vP2 = vecExpandedOutline[j % iSize];
			const Vec3& vQ2 = vecExpandedOutline[(j + 1) % iSize];
			Vec3 vIntersection;

			if (true == IntersectSegments(vP1, vQ1, vP2, vQ2, vIntersection))
			{
				vecResult.push_back(vIntersection);
				i = j; // 교차 구간 skip
				isIntersected = true;
				break;
			}
		}

		if (false == isIntersected && vecExpandedOutline.size() > 2)
		{
			const Vec3& vP2 = vecExpandedOutline[iSize - 1];
			const Vec3& vQ2 = vecExpandedOutline[1];
			Vec3 vIntersection;

			if (true == IntersectSegments(vP1, vQ1, vP2, vQ2, vIntersection))
			{
				vecResult.push_back(vIntersection);
				isIntersected = true;
			}
		}

		if (false == isIntersected)
		{
			vecResult.push_back(vP1);
		}
	}

	return vecResult;
}

_float CNavMeshView::PerpendicularDistance(const Vec3& vPoint, const Vec3& vLineStart, const Vec3& vLineEnd)
{
	_float fDx = vLineEnd.x - vLineStart.x;
	_float fDz = vLineEnd.z - vLineStart.z;

	// Normalize
	// (vLineEnd - vLineStart).Normalize()
	_float fMag = pow(pow(fDx, 2.0f) + pow(fDz, 2.0f), 0.5f);

	if (fMag > 0.0f)
	{
		fDx /= fMag;
		fDz /= fMag;
	}

	_float fPvx = vPoint.x - vLineStart.x;
	_float fPvz = vPoint.z - vLineStart.z;

	// Normalized 방향에 대한 pv길이(내적)
	_float fPvdot = fDx * fPvx + fDz * fPvz;

	// 선분 방향 확장 및 최종 거리 계산
	_float fDsx = fPvdot * fDx;
	_float fDsz = fPvdot * fDz;

	_float fAx = fPvx - fDsx;
	_float fAz = fPvz - fDsz;

	return sqrt(pow(fAx, 2.0f) + pow(fAz, 2.0f));
}

void CNavMeshView::RamerDouglasPeucker(const vector<Vec3>& vecPointList, _float fEpsilon, OUT vector<Vec3>& vecOut)
{
	// 가장 멀리 떨어진 선분 탐색
	_float fDmax = 0.0f;
	size_t iIndex = 0;
	size_t iEnd = vecPointList.size() - 1;
	
	for (size_t i = 1; i < iEnd; i++)
	{
		_float fD = PerpendicularDistance(vecPointList[i], vecPointList[0], vecPointList[iEnd]);

		if (fD > fDmax)
		{
			iIndex = i;
			fDmax = fD;
		}
	}

	// fEpsilon보다 fDmax가 크다면
	if (fDmax > fEpsilon)
	{
		// 재귀 수행
		vector<Vec3> vecRecResults1;
		vector<Vec3> vecRecResults2;
		vector<Vec3> vecFirstLine(vecPointList.begin(), vecPointList.begin() + iIndex + 1);
		vector<Vec3> vecLastLine(vecPointList.begin() + iIndex, vecPointList.end());
		RamerDouglasPeucker(vecFirstLine, fEpsilon, vecRecResults1);
		RamerDouglasPeucker(vecLastLine, fEpsilon, vecRecResults2);

		// 최종 리스트
		vecOut.assign(vecRecResults1.begin(), vecRecResults1.end() - 1);
		vecOut.insert(vecOut.end(), vecRecResults2.begin(), vecRecResults2.end());
	}
	else
	{
		vecOut.clear();
		vecOut.push_back(vecPointList[0]);
		vecOut.push_back(vecPointList[iEnd]);
	}
}

void CNavMeshView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	const POINT& p = CGameInstance::GetInstance()->GetMousePos();
	if (p.x > 1440 || p.x < 0 || p.y > 810 || p.y < 0)
	{
		return;
	}

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		if (m_IsPickingActivated)
		{
			const POINT& p = m_pGameInstance->GetMousePos();
			Pick(p.x, p.y);
		}
		else
		{
			if (nullptr != m_pAgent)
			{
				const POINT& p = m_pGameInstance->GetMousePos();
				m_pAgent->Pick(m_pTerrainBuffer, p.x, p.y);
			}
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

	const POINT& p = m_pGameInstance->GetMousePos();
	if (false == m_pTerrainBuffer->Pick(p.x, p.y, pickPos, fDistance, m_pTerrainBuffer->GetTransform()->WorldMatrix()))
		return false;
	
	BoundingSphere tSphere;
	tSphere.Center = pickPos;
	tSphere.Radius = 2.f;

	if (TRIMODE::DEFAULT == m_eCurrentTriangleMode)
	{
		m_vecPoints.push_back(pickPos);
		m_vecPointSpheres.push_back(tSphere);
		s2cPushBack(m_strPoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));
		++m_iStaticPointCount;

		if (3 <= m_vecPoints.size())
		{
			SafeReleaseTriangle(m_tOut);

			UpdatePointList(m_tIn, m_vecPoints);
			UpdateSegmentList(m_tIn, m_vecPoints);
			UpdateHoleList(m_tIn);
			UpdateRegionList(m_tIn);

			triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);
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

	return true;
}

CellData* CNavMeshView::FindCellByPosition(const Vec3& vPosition)
{
	CellData* pCell = nullptr;

	_int iX = (vPosition.x + gWorldCX * 0.5f) / gGridCX;
	_int iZ = (vPosition.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iKey = iZ * gGridX + iX;

	auto grid = m_umapCellGrids.equal_range(iKey);

	for (auto cell = grid.first; cell != grid.second; ++cell)
	{
		if (false == cell->second->IsOut(vPosition, pCell))
		{
			return cell->second;
		}
	}

	return nullptr;
}

HRESULT CNavMeshView::SaveNvFile()
{
	fs::path strPath("../Bin/Resources/LevelData/" + m_strFilePath + "/");

	if (true == m_vecObstacles.empty())
	{
		MSG_BOX("Fail to Save : You need to Create Obstacles");
	}

	fs::create_directories(strPath);

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Obstacles");
	document->LinkEndChild(root);

	size_t iNumberofFiles = std::distance(fs::directory_iterator(strPath), fs::directory_iterator{});

	tinyxml2::XMLElement* node = nullptr;
	tinyxml2::XMLElement* element = nullptr;

	for (_int i = 0; i < m_vecObstacles.size(); ++i)
	{
		string strName = "Obstacle" + to_string(i);
		node = document->NewElement(strName.c_str());

		root->LinkEndChild(node);
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
				//return E_FAIL;
			}

			element = document->NewElement("GameObject");
			if (nullptr != vecGroundObjects)
			{
				element->SetText(Utils::ToString((*vecGroundObjects)[i]->GetObjectTag()).c_str());
				node->LinkEndChild(element);

				element = document->NewElement("WorldMatrix");
				element->SetAttribute("_11", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._11);
				element->SetAttribute("_12", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._12);
				element->SetAttribute("_13", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._13);
				element->SetAttribute("_14", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._14);
				element->SetAttribute("_21", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._21);
				element->SetAttribute("_22", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._22);
				element->SetAttribute("_23", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._23);
				element->SetAttribute("_24", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._24);
				element->SetAttribute("_31", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._31);
				element->SetAttribute("_32", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._32);
				element->SetAttribute("_33", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._33);
				element->SetAttribute("_34", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._34);
				element->SetAttribute("_41", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._41);
				element->SetAttribute("_42", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._42);
				element->SetAttribute("_43", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._43);
				element->SetAttribute("_44", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._44);
			}
			else
				element->SetText(".");
			
			node->LinkEndChild(element);

			element = document->NewElement("InnerPoint");
			element->SetAttribute("X", m_vecObstacles[i]->vInnerPoint.x);
			element->SetAttribute("Y", m_vecObstacles[i]->vInnerPoint.y);
			element->SetAttribute("Z", m_vecObstacles[i]->vInnerPoint.z);
			node->LinkEndChild(element);

			element = document->NewElement("AABBCenter");
			element->SetAttribute("X", m_vecObstacles[i]->tAABB.Center.x);
			element->SetAttribute("Y", m_vecObstacles[i]->tAABB.Center.y);
			element->SetAttribute("Z", m_vecObstacles[i]->tAABB.Center.z);
			node->LinkEndChild(element);
			
			element = document->NewElement("AABBExtents");
			element->SetAttribute("X", m_vecObstacles[i]->tAABB.Extents.x);
			element->SetAttribute("Y", m_vecObstacles[i]->tAABB.Extents.y);
			element->SetAttribute("Z", m_vecObstacles[i]->tAABB.Extents.z);
			node->LinkEndChild(element);

			element = document->NewElement("Points");
			tinyxml2::XMLElement* point = nullptr;
			for (_int j = 0; j < m_vecObstacles[i]->vecPoints.size(); ++j)
			{
				string strName = "Point" + to_string(j);
				point = document->NewElement(strName.c_str());
				point->SetAttribute("X", m_vecObstacles[i]->vecPoints[j].x);
				point->SetAttribute("Y", m_vecObstacles[i]->vecPoints[j].y);
				point->SetAttribute("Z", m_vecObstacles[i]->vecPoints[j].z);
				element->LinkEndChild(point);
			}
			node->LinkEndChild(element);
		}		
	}

	fs::path finalPath;
	do
	{
		finalPath = strPath.generic_string() + "ObstacleData" + to_string(iNumberofFiles++) + ".xml";
	} while (true == fs::directory_entry(finalPath).exists());

	if (tinyxml2::XML_SUCCESS == document->SaveFile(finalPath.generic_string().c_str()))
	{
		s2cPushBack(m_vecDataFiles, finalPath.filename().generic_string());
	}

	return S_OK;
}

HRESULT CNavMeshView::Save3DNvFile()
{
	fs::path strPath("../Bin/Resources/LevelData/" + m_strFilePath + "/");

	fs::create_directories(strPath);

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Obstacles");
	document->LinkEndChild(root);

	size_t iNumberofFiles = std::distance(fs::directory_iterator(strPath), fs::directory_iterator{});

	tinyxml2::XMLElement* node = nullptr;
	tinyxml2::XMLElement* element = nullptr;

	for (_int i = 0; i < m_vecObstacles.size(); ++i)
	{
		string strName = "Obstacle" + to_string(i);
		node = document->NewElement(strName.c_str());

		root->LinkEndChild(node);
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
				//return E_FAIL;
			}

			element = document->NewElement("GameObject");
			if (nullptr != vecGroundObjects)
			{
				element->SetText(Utils::ToString((*vecGroundObjects)[i]->GetObjectTag()).c_str());
				node->LinkEndChild(element);

				element = document->NewElement("WorldMatrix");
				element->SetAttribute("_11", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._11);
				element->SetAttribute("_12", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._12);
				element->SetAttribute("_13", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._13);
				element->SetAttribute("_14", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._14);
				element->SetAttribute("_21", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._21);
				element->SetAttribute("_22", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._22);
				element->SetAttribute("_23", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._23);
				element->SetAttribute("_24", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._24);
				element->SetAttribute("_31", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._31);
				element->SetAttribute("_32", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._32);
				element->SetAttribute("_33", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._33);
				element->SetAttribute("_34", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._34);
				element->SetAttribute("_41", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._41);
				element->SetAttribute("_42", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._42);
				element->SetAttribute("_43", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._43);
				element->SetAttribute("_44", (*vecGroundObjects)[i]->GetTransform()->WorldMatrix()._44);
			}
			else
				element->SetText(".");

			node->LinkEndChild(element);

			element = document->NewElement("InnerPoint");
			element->SetAttribute("X", m_vecObstacles[i]->vInnerPoint.x);
			element->SetAttribute("Y", m_vecObstacles[i]->vInnerPoint.y);
			element->SetAttribute("Z", m_vecObstacles[i]->vInnerPoint.z);
			node->LinkEndChild(element);

			element = document->NewElement("AABBCenter");
			element->SetAttribute("X", m_vecObstacles[i]->tAABB.Center.x);
			element->SetAttribute("Y", m_vecObstacles[i]->tAABB.Center.y);
			element->SetAttribute("Z", m_vecObstacles[i]->tAABB.Center.z);
			node->LinkEndChild(element);

			element = document->NewElement("AABBExtents");
			element->SetAttribute("X", m_vecObstacles[i]->tAABB.Extents.x);
			element->SetAttribute("Y", m_vecObstacles[i]->tAABB.Extents.y);
			element->SetAttribute("Z", m_vecObstacles[i]->tAABB.Extents.z);
			node->LinkEndChild(element);

			element = document->NewElement("Points");
			tinyxml2::XMLElement* point = nullptr;
			for (_int j = 0; j < m_vecObstacles[i]->vecPoints.size(); ++j)
			{
				string strName = "Point" + to_string(j);
				point = document->NewElement(strName.c_str());
				point->SetAttribute("X", m_vecObstacles[i]->vecPoints[j].x);
				point->SetAttribute("Y", m_vecObstacles[i]->vecPoints[j].y);
				point->SetAttribute("Z", m_vecObstacles[i]->vecPoints[j].z);
				element->LinkEndChild(point);
			}
			node->LinkEndChild(element);
		}
	}

	node = document->NewElement("StaticPoints");
	root->LinkEndChild(node);

	element = document->NewElement("Points");
	tinyxml2::XMLElement* point = nullptr;
	for (_int i = 0; i < m_vecPoints.size(); ++i)
	{
		string strName = "Point" + to_string(i);
		point = document->NewElement(strName.c_str());
		point->SetAttribute("X", m_vecPoints[i].x);
		point->SetAttribute("Y", m_vecPoints[i].y);
		point->SetAttribute("Z", m_vecPoints[i].z);
		element->LinkEndChild(point);
	}
	node->LinkEndChild(element);

	node = document->NewElement("StaticSegments");
	root->LinkEndChild(node);

	element = document->NewElement("Segments");
	tinyxml2::XMLElement* segment = nullptr;
	for (_int i = 0; i < m_vecSegments.size(); ++i)
	{
		string strName = "Segment" + to_string(i);
		segment = document->NewElement(strName.c_str());
		segment->SetAttribute("Idx", m_vecSegments[i]);
		element->LinkEndChild(segment);
	}
	node->LinkEndChild(element);

	fs::path finalPath;
	do
	{
		finalPath = strPath.generic_string() + "3DNavData" + to_string(iNumberofFiles++) + ".xml";
	} while (true == fs::directory_entry(finalPath).exists());

	if (tinyxml2::XML_SUCCESS == document->SaveFile(finalPath.generic_string().c_str()))
	{
		s2cPushBack(m_vecDataFiles, finalPath.filename().generic_string());
	}

	return S_OK;
}

HRESULT CNavMeshView::LoadNvFile()
{
	fs::path strPath("../Bin/Resources/LevelData/" + m_strFilePath + "/" + m_vecDataFiles[m_file_Current]);
	
	fs::directory_entry file(strPath);

	if (false == file.is_regular_file() || ".xml" != file.path().extension())
	{
		MSG_BOX("Failed to Load");
		return E_FAIL;
	}

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
	tinyxml2::XMLError error = document->LoadFile(file.path().generic_string().c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = nullptr;
	root = document->FirstChildElement();
	tinyxml2::XMLElement* node = nullptr;
	node = root->FirstChildElement();

	if (nullptr == node)
	{
		MSG_BOX("Fail to Load");
		return E_FAIL;
	}

	Reset();
	InitialSetting();

	// Obstacles
	tinyxml2::XMLElement* element = nullptr;
	while (nullptr != node)
	{
		Matrix matWorld;
		wstring strObjectTag;

		element = node->FirstChildElement();
		strObjectTag = Utils::ToWString(element->GetText());

		if (TEXT(".") != strObjectTag)
		{
			element = element->NextSiblingElement();
			matWorld._11 = element->FloatAttribute("_11");
			matWorld._12 = element->FloatAttribute("_12");
			matWorld._13 = element->FloatAttribute("_13");
			matWorld._14 = element->FloatAttribute("_14");
			matWorld._21 = element->FloatAttribute("_21");
			matWorld._22 = element->FloatAttribute("_22");
			matWorld._23 = element->FloatAttribute("_23");
			matWorld._24 = element->FloatAttribute("_24");
			matWorld._31 = element->FloatAttribute("_31");
			matWorld._32 = element->FloatAttribute("_32");
			matWorld._33 = element->FloatAttribute("_33");
			matWorld._34 = element->FloatAttribute("_34");
			matWorld._41 = element->FloatAttribute("_41");
			matWorld._42 = element->FloatAttribute("_42");
			matWorld._43 = element->FloatAttribute("_43");
			matWorld._44 = element->FloatAttribute("_44");

			m_pMediator->OnNotifiedPlaceObject(strObjectTag, matWorld);			
		}

		Obst* pObst = new Obst;

		// InnerPoint
		element = element->NextSiblingElement();
		pObst->vInnerPoint.x = element->FloatAttribute("X");
		pObst->vInnerPoint.y = element->FloatAttribute("Y");
		pObst->vInnerPoint.z = element->FloatAttribute("Z");
		
		// AABB
		element = element->NextSiblingElement();
		pObst->tAABB.Center.x = element->FloatAttribute("X");
		pObst->tAABB.Center.y = element->FloatAttribute("Y");
		pObst->tAABB.Center.z = element->FloatAttribute("Z");

		element = element->NextSiblingElement();
		pObst->tAABB.Extents.x = element->FloatAttribute("X") + 0.05f;
		pObst->tAABB.Extents.y = element->FloatAttribute("Y");
		pObst->tAABB.Extents.z = element->FloatAttribute("Z") + 0.05f;
		
		// Points
		element = element->NextSiblingElement();
		tinyxml2::XMLElement* point = element->FirstChildElement();
		while (nullptr != point)
		{
			Vec3 vPoint;
			vPoint.x = point->FloatAttribute("X");
			vPoint.y = point->FloatAttribute("Y");
			vPoint.z = point->FloatAttribute("Z");

			pObst->vecPoints.push_back(vPoint);

			point = point->NextSiblingElement();
		}

		m_vecObstacles.push_back(pObst);
		s2cPushBack(m_strObstacles, to_string(m_vecObstacles.back()->vInnerPoint.x) + ", " + to_string(m_vecObstacles.back()->vInnerPoint.z));

		node = node->NextSiblingElement();
	}

	for (auto pObst : m_vecObstacles)
	{
		for (auto& vPoint : pObst->vecPoints)
		{
			m_vecPoints.push_back(vPoint);
		}
	}

	SafeReleaseTriangle(m_tIn);
	SafeReleaseTriangle(m_tOut);

	UpdatePointList(m_tIn, m_vecPoints);
	UpdateSegmentList(m_tIn, m_vecPoints);
	UpdateHoleList(m_tIn);
	UpdateRegionList(m_tIn);

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);
	
	if (FAILED(BakeNavMesh()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNavMeshView::Load3DNvFile()
{
	fs::path strPath("../Bin/Resources/LevelData/" + m_strFilePath + "/" + m_vecDataFiles[m_file_Current]);

	fs::directory_entry file(strPath);

	if (false == file.is_regular_file() || ".xml" != file.path().extension())
	{
		MSG_BOX("Failed to Load");
		return E_FAIL;
	}

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
	tinyxml2::XMLError error = document->LoadFile(file.path().generic_string().c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = nullptr;
	root = document->FirstChildElement();
	tinyxml2::XMLElement* node = nullptr;
	node = root->FirstChildElement();

	if (nullptr == node)
	{
		MSG_BOX("Fail to Load");
		return E_FAIL;
	}

	Reset();
	InitialSetting();

	// Obstacles
	tinyxml2::XMLElement* element = nullptr;
	while (nullptr != node)
	{
		Matrix matWorld;
		wstring strObjectTag;

		element = node->FirstChildElement();
		strObjectTag = Utils::ToWString(element->GetText());

		if (TEXT(".") != strObjectTag)
		{
			element = element->NextSiblingElement();
			matWorld._11 = element->FloatAttribute("_11");
			matWorld._12 = element->FloatAttribute("_12");
			matWorld._13 = element->FloatAttribute("_13");
			matWorld._14 = element->FloatAttribute("_14");
			matWorld._21 = element->FloatAttribute("_21");
			matWorld._22 = element->FloatAttribute("_22");
			matWorld._23 = element->FloatAttribute("_23");
			matWorld._24 = element->FloatAttribute("_24");
			matWorld._31 = element->FloatAttribute("_31");
			matWorld._32 = element->FloatAttribute("_32");
			matWorld._33 = element->FloatAttribute("_33");
			matWorld._34 = element->FloatAttribute("_34");
			matWorld._41 = element->FloatAttribute("_41");
			matWorld._42 = element->FloatAttribute("_42");
			matWorld._43 = element->FloatAttribute("_43");
			matWorld._44 = element->FloatAttribute("_44");

			m_pMediator->OnNotifiedPlaceObject(strObjectTag, matWorld);
		}

		Obst* pObst = new Obst;

		// InnerPoint
		element = element->NextSiblingElement();
		pObst->vInnerPoint.x = element->FloatAttribute("X");
		pObst->vInnerPoint.y = element->FloatAttribute("Y");
		pObst->vInnerPoint.z = element->FloatAttribute("Z");

		// AABB
		element = element->NextSiblingElement();
		pObst->tAABB.Center.x = element->FloatAttribute("X");
		pObst->tAABB.Center.y = element->FloatAttribute("Y");
		pObst->tAABB.Center.z = element->FloatAttribute("Z");

		element = element->NextSiblingElement();
		pObst->tAABB.Extents.x = element->FloatAttribute("X") + 0.05f;
		pObst->tAABB.Extents.y = element->FloatAttribute("Y");
		pObst->tAABB.Extents.z = element->FloatAttribute("Z") + 0.05f;

		// Points
		element = element->NextSiblingElement();
		tinyxml2::XMLElement* point = element->FirstChildElement();
		while (nullptr != point)
		{
			Vec3 vPoint;
			vPoint.x = point->FloatAttribute("X");
			vPoint.y = point->FloatAttribute("Y");
			vPoint.z = point->FloatAttribute("Z");

			pObst->vecPoints.push_back(vPoint);

			point = point->NextSiblingElement();
		}

		m_vecObstacles.push_back(pObst);
		s2cPushBack(m_strObstacles, to_string(m_vecObstacles.back()->vInnerPoint.x) + ", " + to_string(m_vecObstacles.back()->vInnerPoint.z));

		node = node->NextSiblingElement();
	}

	for (auto pObst : m_vecObstacles)
	{
		for (auto& vPoint : pObst->vecPoints)
		{
			m_vecPoints.push_back(vPoint);
		}
	}

	SafeReleaseTriangle(m_tIn);
	SafeReleaseTriangle(m_tOut);

	UpdatePointList(m_tIn, m_vecPoints);
	UpdateSegmentList(m_tIn, m_vecPoints);
	UpdateHoleList(m_tIn);
	UpdateRegionList(m_tIn);

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);

	if (FAILED(BakeNavMesh()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNavMeshView::DeleteNvFile()
{
	fs::path strPath("../Bin/Resources/LevelData/" + m_strFilePath + "/");
	
	if (true == m_vecDataFiles.empty())
	{
		MSG_BOX("Failed : DataFile Empty");
		return E_FAIL;
	}

	strPath += m_vecDataFiles[m_file_Current];
	if (false == fs::remove(strPath))
	{
		MSG_BOX("Failed to Delete File");
		return E_FAIL;
	}
		
	MSG_BOX("Succeeded to Delete File");

	Safe_Delete(m_vecDataFiles[m_file_Current]);
	m_vecDataFiles.erase(m_vecDataFiles.begin() + m_file_Current);

	if (m_vecDataFiles.size() - 1 < m_file_Current)
	{
		m_file_Current = ::max(0, (_int)m_vecDataFiles.size() - 1);
	}

	return S_OK;
}

HRESULT CNavMeshView::RefreshNvFile()
{
	for (auto szFilename : m_vecDataFiles)
	{
		Safe_Delete(szFilename);
	}
	m_vecDataFiles.clear();

	fs::path strPath = fs::path("../Bin/Resources/LevelData/" + m_strFilePath + "/");

	if (fs::exists(strPath) && fs::is_directory(strPath))
	{
		for (const auto& entry : fs::directory_iterator(strPath))
		{
			if (entry.is_regular_file())
			{
				s2cPushBack(m_vecDataFiles, entry.path().filename().generic_string());
			}
		}
	}

	return S_OK;
}

void CNavMeshView::InfoView()
{
	ImGui::Text("This window has some useful function for Objects in Level.");
	ImGui::NewLine();

	ImGui::Text("FPS : %1f", 1.0f / ImGui::GetIO().DeltaTime);
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

	if (ImGui::BeginCombo("Mode", m_strCurrentTriangleMode.c_str()))
	{
		static const _char* szMode[3] = { "Default", "Obstacle", "Region" };

		for (uint8 i = 0; i < (uint8)TRIMODE::MODE_END; ++i)
		{
			_bool isSelected = (0 == strcmp(m_strCurrentTriangleMode.c_str(), szMode[i]));
			
			if (ImGui::Selectable(szMode[i], isSelected))
			{
				m_strCurrentTriangleMode = szMode[i];
				m_eCurrentTriangleMode = (TRIMODE)i;
				m_item_Current = 0;
			}

			if (true == isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	/*ImGui::InputFloat("SlopeMax (degree)", &m_fSlopeDegree);
	ImGui::InputFloat("ClimbMax (height)", &m_fMaxClimb);
	ImGui::InputFloat("AreaMin (degree)", &m_fMinArea);
	ImGui::InputFloat3("AABB Center", (_float*)&m_tNavMeshBoundVolume.Center);
	ImGui::InputFloat3("AABB Extent", (_float*)&m_tNavMeshBoundVolume.Extents);*/

	if (ImGui::Button("BakeNav"))
	{
		BakeNavMesh();
	}

	static _bool bBakeSingle = false;
	if (true == bBakeSingle && 1 == m_vecObstacles.size())
	{		
		if (ImGui::Button("SaveSingleObst"))
		{
			if (SUCCEEDED(SaveObstacleLocalOutline(m_vecObstacles[0], m_strObstacles[0])))
			{
				MSG_BOX("Succeed to Save Single Obstacle");
			}
			else
			{
				MSG_BOX("Failed to Save Single Obstacle");
			}
		}
	}
	else if(false == bBakeSingle && 0 == m_vecObstacles.size())
	{
		if (ImGui::Button("BakeSingleObst"))
		{
			if (SUCCEEDED(BakeSingleObstacleData()))
			{
				MSG_BOX("Succeed to Save Single Obstacle");
				bBakeSingle = true;
			}
			else
			{
				MSG_BOX("Failed to Save Single Obstacle");
			}
		}
		
	}ImGui::SameLine();

	if (0 == m_vecObstacles.size())
	{
		if (ImGui::Button("BakeHeightMapObst"))
		{
			if (SUCCEEDED(BakeHeightMapObstacles()))
			{
				MSG_BOX("Succeed to Bake Height Obstacles");
			}
			else
			{
				MSG_BOX("Failed to Bake Height Obstacles");
			}
		}
	}ImGui::NewLine();

	if (ImGui::Button("Bake3DNav"))
	{
		if (SUCCEEDED(BakeHeightMap3D()))
		{
			MSG_BOX("Succeed to Bake 3D Terrain");
		}
		else
		{
			MSG_BOX("Failed to Bake 3D Terrain");
		}
	}ImGui::NewLine();

	ImGui::ListBox("Data Files", &m_file_Current, m_vecDataFiles.data(), m_vecDataFiles.size(), 3);

	if (ImGui::Button("SaveNav"))
	{
		SaveNvFile();
	}ImGui::SameLine();

	if (ImGui::Button("LoadNav"))
	{
		LoadNvFile();
	}ImGui::SameLine();

	if (ImGui::Button("DeleteFile"))
	{
		DeleteNvFile();
	}ImGui::SameLine();

	if (ImGui::Button("RefreshFile"))
	{
		RefreshNvFile();
	}ImGui::NewLine();

	if (ImGui::Button("Save3DNav"))
	{
		Save3DNvFile();
	}
}

void CNavMeshView::PointsGroup()
{
	if (ImGui::TreeNode("Points"))
	{
		ImGui::ListBox("", &m_item_Current, m_strPoints.data(), m_strPoints.size(), 3);
		if (ImGui::Button("PopBackPoint"))
		{
			if (m_vecPoints.empty())
				return;

			m_strPoints.pop_back();
			m_vecPoints.pop_back();
			m_vecPointSpheres.pop_back();
		}
		ImGui::TreePop();
	}
}

void CNavMeshView::ObstaclesGroup()
{
	if (ImGui::TreeNode("Obstacles"))
	{
		ImGui::ListBox("ObstaclePoints", &m_item_Current, m_strObstaclePoints.data(), m_strObstaclePoints.size(), 3);

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
			if (ImGui::Button("CreateDynamic"))
			{
				Obst* pObst = new Obst;

				TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
				for (auto vPoint : m_vecObstaclePoints)
				{
					if (fMaxX < vPoint.x) fMaxX = vPoint.x;
					if (fMinX > vPoint.x) fMinX = vPoint.x;

					if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
					if (fMinZ > vPoint.z) fMinZ = vPoint.z;

					pObst->vecPoints.push_back(vPoint);
				}

				const _float fAABBOffset = 0.05f;
				Vec3 vAABBCenter((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
				Vec3 vAABBExtent((fMaxX - fMinX) * 0.5f + fAABBOffset, 10.0f, (fMaxZ - fMinZ) * 0.5f + fAABBOffset);
				pObst->tAABB = BoundingBox(vAABBCenter, vAABBExtent);

				SetPolygonHoleCenter(*pObst);

				m_vecObstacles.push_back(pObst);
				s2cPushBack(m_strObstacles, to_string(m_vecObstacles.back()->vInnerPoint.x) + ", " + to_string(m_vecObstacles.back()->vInnerPoint.z));

				DynamicCreate(*pObst);

				for (auto obst : m_strObstaclePoints)
				{
					delete obst;
				}

				m_strObstaclePoints.clear();
				m_vecObstaclePoints.clear();
			}
		}
	
		ImGui::ListBox("", &m_item_Current, m_strObstacles.data(), m_strObstacles.size(), 3);

		if (false == m_strObstacles.empty())
		{
			ImGui::NewLine();
			if (ImGui::Button("DeleteObstacle"))
			{
				DynamicDelete(*m_vecObstacles[m_item_Current]);
				auto iter = m_vecObstacles.begin() + m_item_Current;
				auto iterStr = m_strObstacles.begin() + m_item_Current;
				m_vecObstacles.erase(iter);
				m_strObstacles.erase(iterStr);

				if (m_vecObstacles.size() - 1 < m_item_Current)
				{
					m_item_Current = ::max(0, (_int)m_vecObstacles.size() - 1);
				}
			}
		}
		ImGui::TreePop();
	}
}
// https://gdcvault.com/play/1014514/AI-Navigation-It-s-Not
void CNavMeshView::CellGroup()
{
	if (ImGui::TreeNode("Cells"))
	{
		ImGui::ListBox("", &m_item_Current, m_strCells.data(), m_strCells.size(), 3);
		if (ImGui::Button("PopBackCell"))
		{
			if (m_vecCells.empty())
				return;

			m_strCells.pop_back();
			m_vecCells.pop_back();

			auto iter = m_vecPointSpheres.end() - 3 - m_vecPoints.size();
			for (_int i = 0; i < 3; ++i)
				iter = m_vecPointSpheres.erase(iter);
		}
		ImGui::TreePop();
	}
}

HRESULT CNavMeshView::InitialSetting()
{
	m_vecPoints.push_back(Vec3(-512.0f, 0.f, -512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(-512.0f, 0.f, -512.0f), 2.f));

	m_vecPoints.push_back(Vec3(+512.0f, 0.f, -512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(+512.0f, 0.f, -512.0f), 2.f));

	m_vecPoints.push_back(Vec3(+512.0f, 0.f, +512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(+512.0f, 0.f, +512.0f), 2.f));

	m_vecPoints.push_back(Vec3(-512.0f, 0.f, +512.0f));
	m_vecPointSpheres.push_back(BoundingSphere(Vec3(-512.0f, 0.f, +512.0f), 2.f));

	m_iStaticPointCount = m_vecPoints.size();

	if (FAILED(UpdatePointList(m_tIn, m_vecPoints)))
		return E_FAIL;

	if (FAILED(UpdateSegmentList(m_tIn, m_vecPoints)))
		return E_FAIL;

	if (FAILED(UpdateHoleList(m_tIn)))
		return E_FAIL;

	if (FAILED(UpdateRegionList(m_tIn)))
		return E_FAIL;

	triangulate(m_szTriswitches, &m_tIn, &m_tOut, nullptr);

	return S_OK;
}

HRESULT CNavMeshView::Reset()
{
	SafeReleaseTriangle(m_tIn);
	SafeReleaseTriangle(m_tOut);
	
	if (m_tIn.holelist)
	{
		free(m_tIn.holelist);
		m_tIn.holelist = nullptr;
	}

	if (m_tIn.regionlist)
	{
		free(m_tIn.regionlist);
		m_tIn.regionlist = nullptr;
	}

	m_vecPoints.clear();
	m_vecSegments.clear();

	for (auto cell : m_vecCells)
	{
		Safe_Delete(cell);
	}
	m_vecCells.clear();

	for (auto cell : m_strCells)
	{
		Safe_Delete(cell);
	}
	m_strCells.clear();

	m_vecObstacles.clear();

	for (auto obstacle : m_strObstacles)
	{
		Safe_Delete(obstacle);
	}
	m_strObstacles.clear();

	m_vecObstaclePoints.clear();

	for (auto obstacle : m_strObstaclePoints)
	{
		Safe_Delete(obstacle);
	}
	m_strObstaclePoints.clear();

	m_item_Current = 0;

	m_vecPointSpheres.clear();
	m_vecObstaclePointSpheres.clear();
	m_vecRegionSpheres.clear();

	return S_OK;
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
	//if (tTriangle.holelist)				{ free(tTriangle.holelist);					tTriangle.holelist = nullptr; }
	//if (tTriangle.regionlist)				{ free(tTriangle.regionlist);				tTriangle.regionlist = nullptr; }
	if (tTriangle.edgelist)					{ free(tTriangle.edgelist);					tTriangle.edgelist = nullptr; }
	if (tTriangle.edgemarkerlist)			{ free(tTriangle.edgemarkerlist);			tTriangle.edgemarkerlist = nullptr; }
	if (tTriangle.normlist)					{ free(tTriangle.normlist);					tTriangle.normlist = nullptr; }

	return S_OK;
}

HRESULT CNavMeshView::SaveObstacleLocalOutline(const Obst* const pObst, string strName)
{
	fs::path strPath("../Bin/Resources/NavObstacles/");

	fs::create_directories(strPath);

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement(strName.c_str());
	document->LinkEndChild(root);

	tinyxml2::XMLElement* node = nullptr;
	tinyxml2::XMLElement* element = nullptr;

	node = document->NewElement(strName.c_str());

	root->LinkEndChild(node);
	{
		element = document->NewElement("InnerPoint");
		element->SetAttribute("X", pObst->vInnerPoint.x);
		element->SetAttribute("Y", pObst->vInnerPoint.y);
		element->SetAttribute("Z", pObst->vInnerPoint.z);
		node->LinkEndChild(element);

		element = document->NewElement("AABBCenter");
		element->SetAttribute("X", pObst->tAABB.Center.x);
		element->SetAttribute("Y", pObst->tAABB.Center.y);
		element->SetAttribute("Z", pObst->tAABB.Center.z);
		node->LinkEndChild(element);

		element = document->NewElement("AABBExtents");
		element->SetAttribute("X", pObst->tAABB.Extents.x);
		element->SetAttribute("Y", pObst->tAABB.Extents.y);
		element->SetAttribute("Z", pObst->tAABB.Extents.z);
		node->LinkEndChild(element);

		element = document->NewElement("Points");
		tinyxml2::XMLElement* point = nullptr;
		for (_int j = 0; j < pObst->vecPoints.size(); ++j)
		{
			string strName = "Point" + to_string(j);
			point = document->NewElement(strName.c_str());
			point->SetAttribute("X", pObst->vecPoints[j].x);
			point->SetAttribute("Y", pObst->vecPoints[j].y);
			point->SetAttribute("Z", pObst->vecPoints[j].z);
			element->LinkEndChild(point);
		}
		node->LinkEndChild(element);
	}

	fs::path finalPath = strPath.generic_string() + strName + ".xml";

	return (tinyxml2::XML_SUCCESS == document->SaveFile(finalPath.generic_string().c_str())) ? S_OK : E_FAIL;
}

HRESULT CNavMeshView::LoadObstacleOutlineData()
{
	fs::path strDirectoryPath("../Bin/Resources/NavObstacles/");

	string strFileName;

	if (false == fs::exists(strDirectoryPath) || false == fs::is_directory(strDirectoryPath))
	{
		return E_FAIL;
	}

	for (const auto& entry : fs::directory_iterator(strDirectoryPath))
	{
		if (false == entry.is_regular_file() || ".xml" != entry.path().extension())
		{
			MSG_BOX("Failed to Load");
			return E_FAIL;
		}

		shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
		tinyxml2::XMLError error = document->LoadFile(entry.path().generic_string().c_str());
		assert(error == tinyxml2::XML_SUCCESS);

		tinyxml2::XMLElement* root = nullptr;
		root = document->FirstChildElement();
		tinyxml2::XMLElement* node = nullptr;
		node = root->FirstChildElement();

		if (nullptr == node)
		{
			MSG_BOX("Fail to Load");
			return E_FAIL;
		}

		// Obstacles
		tinyxml2::XMLElement* element = nullptr;
		while (nullptr != node)
		{
			Obst* pObst = new Obst;

			// InnerPoint
			element = node->FirstChildElement();
			pObst->vInnerPoint.x = element->FloatAttribute("X");
			pObst->vInnerPoint.y = element->FloatAttribute("Y");
			pObst->vInnerPoint.z = element->FloatAttribute("Z");

			// AABB
			element = element->NextSiblingElement();
			pObst->tAABB.Center.x = element->FloatAttribute("X");
			pObst->tAABB.Center.y = element->FloatAttribute("Y");
			pObst->tAABB.Center.z = element->FloatAttribute("Z");

			element = element->NextSiblingElement();
			pObst->tAABB.Extents.x = element->FloatAttribute("X");
			pObst->tAABB.Extents.y = element->FloatAttribute("Y");
			pObst->tAABB.Extents.z = element->FloatAttribute("Z");

			// Points
			element = element->NextSiblingElement();
			tinyxml2::XMLElement* point = element->FirstChildElement();
			while (nullptr != point)
			{
				Vec3 vPoint;
				vPoint.x = point->FloatAttribute("X");
				vPoint.y = point->FloatAttribute("Y");
				vPoint.z = point->FloatAttribute("Z");

				pObst->vecPoints.push_back(vPoint);

				point = point->NextSiblingElement();
			}

			m_mapObstaclePrefabs.emplace(entry.path().filename().stem().generic_wstring(), *pObst);

			Safe_Delete(pObst);

			node = node->NextSiblingElement();
		}
	}
	
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

	for (auto cell : m_vecCells)
	{
		Safe_Delete(cell);
	}
	m_vecCells.clear();
	
	for (auto cell : m_strCells)
	{
		Safe_Delete(cell);
	}
	m_strCells.clear();

	for (auto obst : m_vecObstacles)
	{
		Safe_Delete(obst);
	}
	m_vecObstacles.clear();
	
	for (auto obst : m_strObstacles)
	{
		Safe_Delete(obst);
	}
	m_strObstacles.clear();

	m_umapCellGrids.clear();
	m_umapObstGrids.clear();
}