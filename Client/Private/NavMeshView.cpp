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
#include <boost/polygon/voronoi.hpp>

namespace boost
{
	namespace polygon
	{
		template <>
		struct geometry_concept<VDPoint> { typedef point_concept type; };

		template <>
		struct point_traits<VDPoint>
		{
			typedef int coordinate_type;

			static inline coordinate_type get(const VDPoint& point, orientation_2d orient)
			{
				return (orient == HORIZONTAL) ? point.x : point.y;
			}
		};
	}
}

using namespace boost::polygon;

struct CNavMeshView::CellData
{
	array<Vec3, POINT_END> vPoints = { Vec3::Zero, Vec3::Zero, Vec3::Zero };
	array<CellData*, LINE_END> arrNeighbors = { nullptr, nullptr, nullptr };

	// cache
	_bool	isOverSlope = false;
	_int	iSlope = -1;

	void CW()
	{
		Vec3 vA(vPoints[POINT_A].x, 0.f, vPoints[POINT_A].z);
		Vec3 vB(vPoints[POINT_B].x, 0.f, vPoints[POINT_B].z);
		Vec3 vC(vPoints[POINT_C].x, 0.f, vPoints[POINT_C].z);

		Vec3 vFlatAB = Vec3(vB - vA);
		Vec3 vFlatBC = Vec3(vC - vB);
		Vec3 vResult;
		vFlatAB.Cross(vFlatBC, vResult);

		if (vResult.y < 0.f)
		{
			::swap(vPoints[1], vPoints[2]);
		}
	}

	_bool ComparePoints(const Vec3& pSour, const Vec3& pDest, CellData* pNeighbor = nullptr)
	{
		for (_int i = POINT_A; i < POINT_END; ++i)
		{
			if (vPoints[i] == pSour)
			{
				for (_int j = POINT_A; j < POINT_END; ++j)
				{
					if (j != i && vPoints[j] == pDest)
					{
						if (nullptr != pNeighbor)
						{
							if (POINT_A == i)
							{
								if (nullptr == arrNeighbors[LINE_AB] && POINT_B == j)
								{
									arrNeighbors[LINE_AB] = pNeighbor;
								}
								else if (nullptr == arrNeighbors[LINE_CA] && POINT_C == j)
								{
									arrNeighbors[LINE_CA] = pNeighbor;
								}
							}
							else if (POINT_B == i)
							{
								if (nullptr == arrNeighbors[LINE_AB] && POINT_A == j)
								{
									arrNeighbors[LINE_AB] = pNeighbor;
								}
								else if (nullptr == arrNeighbors[LINE_CA] && POINT_C == j)
								{
									arrNeighbors[LINE_CA] = pNeighbor;
								}
							}
							else
							{
								if (nullptr == arrNeighbors[LINE_CA] && POINT_A == j)
								{
									arrNeighbors[LINE_CA] = pNeighbor;
								}
								else if (nullptr == arrNeighbors[LINE_BC] && POINT_B == j)
								{
									arrNeighbors[LINE_BC] = pNeighbor;
								}
							}
						}

						return true;
					}
				}
			}
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

	return S_OK;
}

HRESULT CNavMeshView::Tick()
{
	Input();

	ImGui::Begin("NavMeshTool");

	InfoView();
	PointGroup();
	CellGroup();

	ImGui::End();

	return S_OK;
}

HRESULT CNavMeshView::LateTick()
{
	return S_OK;
}

HRESULT CNavMeshView::DebugRender()
{
	DebugRenderLegacy();

	/*m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	if (!m_vecCells.empty())
	{
		for (_int i = 0; i < m_vecCells.size(); ++i)
		{
			Vec3 vP0 = m_vecCells[i]->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP1 = m_vecCells[i]->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
			Vec3 vP2 = m_vecCells[i]->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

			m_pBatch->Begin();
			DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Cyan);
			m_pBatch->End();
		}
	}

	m_pBatch->Begin();
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
	//for (auto& pSour : vecCells)
	for (_int sour = 0; sour < vecCells.size(); ++sour)
	{
		//for (auto& pDest : vecCells)
		for (_int dest = 0; dest < vecCells.size(); ++dest)
		{
			if (vecCells[sour] == vecCells[dest])
			{
				continue;
			}

			if (nullptr == vecCells[sour]->arrNeighbors[LINE_AB] && true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_A], vecCells[sour]->vPoints[POINT_B], vecCells[sour]))
			{
				vecCells[sour]->arrNeighbors[LINE_AB] = vecCells[dest];
			}
			else if (nullptr == vecCells[sour]->arrNeighbors[LINE_BC] && true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_B], vecCells[sour]->vPoints[POINT_C], vecCells[sour]))
			{
				vecCells[sour]->arrNeighbors[LINE_BC] = vecCells[dest];
			}
			else if (nullptr == vecCells[sour]->arrNeighbors[LINE_CA] && true == vecCells[dest]->ComparePoints(vecCells[sour]->vPoints[POINT_C], vecCells[sour]->vPoints[POINT_A], vecCells[sour]))
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

HRESULT CNavMeshView::CreateVoronoi()
{
	// VD
	m_vecVDCaches.clear();
	m_vecVDPoints.clear();

	for (auto& point : m_vecPoints)
	{
		m_vecVDCaches.push_back(VDPoint(point.x, point.z));
	}

	voronoi_diagram<_double> VD;
	construct_voronoi(m_vecVDCaches.begin(), m_vecVDCaches.end(), &VD);

	for (auto iter = VD.edges().begin(); iter != VD.edges().end(); ++iter)
	{
		if (iter->is_finite())
		{
			auto v0 = iter->vertex0();
			auto v1 = iter->vertex1();

			m_vecVDPoints.push_back(Vec3((_float)v0->x(), 0.05f, (_float)v0->y()));
			m_vecVDPoints.push_back(Vec3((_float)v1->x(), 0.05f, (_float)v1->y()));
		}
	}

	return S_OK;
}

HRESULT CNavMeshView::DebugRenderLegacy()
{
	m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

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
			_float3 vP0 = m_vecCells[i]->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
			_float3 vP1 = m_vecCells[i]->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
			_float3 vP2 = m_vecCells[i]->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

			m_pBatch->Begin();
			DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Cyan);
			m_pBatch->End();
		}

		_float3 vP0 = m_vecCells[m_Item_Current]->vPoints[0] + Vec3(0.f, 0.05f, 0.f);
		_float3 vP1 = m_vecCells[m_Item_Current]->vPoints[1] + Vec3(0.f, 0.05f, 0.f);
		_float3 vP2 = m_vecCells[m_Item_Current]->vPoints[2] + Vec3(0.f, 0.05f, 0.f);

		m_pBatch->Begin();
		DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Coral);
		m_pBatch->End();
	}*/

	if (1 < m_vecPoints.size())
	{
		for (_int i = 0; i < m_vecPoints.size() - 1; ++i)
		{
			m_pBatch->Begin();
			m_pBatch->DrawLine(VertexPositionColor(m_vecPoints[i], Colors::Lime), VertexPositionColor(m_vecPoints[i + 1], Colors::Lime));
			m_pBatch->End();
		}
	}

	if (false == m_vecSphere.empty())
	{
		for (auto& iter : m_vecSphere)
		{
			BoundingSphere tS(iter->Center + 0.05f * Vec3::UnitY, 0.5f);

			m_pBatch->Begin();
			DX::Draw(m_pBatch, tS, Colors::Lime);
			m_pBatch->End();
		}
	}

	if (1 < m_vecVDPoints.size())
	{
		for (_int i = 0; i < m_vecVDPoints.size() - 1; ++i)
		{
			m_pBatch->Begin();
			m_pBatch->DrawLine(VertexPositionColor(m_vecVDPoints[i], Colors::Cyan), VertexPositionColor(m_vecVDPoints[i + 1], Colors::Cyan));
			m_pBatch->End();
		}
	}

	return S_OK;
}

_bool CNavMeshView::CanClimb()
{
	return _bool();
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

	_bool bSpherePicked = false;
	for (auto& iter : m_vecSphere)
	{
		Ray cRay = Ray(vWorldRayOrigin, vWorldRayDir);

		if (false == cRay.Intersects(*iter, OUT fDistance))
			continue;

		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pickPos = iter->Center;
			bSpherePicked = true;
		}
	}

	if (!bSpherePicked)
	{
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
		{
		TERRAIN_PICKED:
			const POINT& p = m_pGameInstance->GetMousePos();
			m_pTerrainBuffer->Pick(p.x, p.y, pickPos, fDistance, m_pTerrainBuffer->GetTransform()->WorldMatrix());
		}
	}

	m_vecPoints.push_back(pickPos);
	BoundingSphere* tSphere = new BoundingSphere;
	tSphere->Center = pickPos;
	tSphere->Radius = 1.f;
	m_vecSphere.push_back(tSphere);

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
	else*/
		s2cPushBack(m_strPoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));

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
			BoundingSphere* tSphere = new BoundingSphere;
			tSphere->Center = tCellData->vPoints[i];
			tSphere->Radius = 1.f;
			m_vecSphere.push_back(tSphere);
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

	ImGui::InputFloat("SlopeMax (degree)", &m_fSlopeDegree);
	ImGui::InputFloat("ClimbMax (height)", &m_fMaxClimb);
	ImGui::InputFloat("AreaMin (degree)", &m_fMinArea);
	ImGui::InputFloat3("AABB Center", (_float*)&m_tNavMeshBoundVolume.Center);
	ImGui::InputFloat3("AABB Extent", (_float*)&m_tNavMeshBoundVolume.Extents);

	/*static Vec3 vOBBRotation = Vec3(0.0f, 0.0f, 0.0f);
	ImGui::InputFloat3("OBB Rotation", (_float*)&vOBBRotation);*/

	if (ImGui::Button("BakeNav"))
	{
		BakeNavMesh();
	}ImGui::SameLine();
	
	if (ImGui::Button("MakeVoronoi"))
	{
		CreateVoronoi();
	}ImGui::NewLine();
	
	if (ImGui::Button("SaveNav"))
	{
		Save();
	}ImGui::SameLine();
	if (ImGui::Button("LoadNav"))
	{
		Load();
	}
}

void CNavMeshView::PointGroup()
{
	ImGui::ListBox("Points", &m_Point_Current, m_strPoints.data(), m_strPoints.size(), 3);
	if (ImGui::Button("PopBackPoint"))
	{
		if (m_vecPoints.empty())
			return;

		m_strPoints.pop_back();
		m_vecPoints.pop_back();
		m_vecSphere.pop_back();
	}
}

void CNavMeshView::CellGroup()
{
	ImGui::ListBox("Cells", &m_Item_Current, m_strCells.data(), m_strCells.size(), 3);
	if (ImGui::Button("PopBackCell"))
	{
		if (m_vecCells.empty())
			return;

		m_strCells.pop_back();
		m_vecCells.pop_back();

		auto iter = m_vecSphere.end() - 3 - m_vecPoints.size();
		for(_int i = 0; i < 3; ++i)
			iter = m_vecSphere.erase(iter);
	}
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
}