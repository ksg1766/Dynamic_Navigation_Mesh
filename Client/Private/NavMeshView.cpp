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

struct CNavMeshView::CellData
{
	_float3 vPoints[3] = { _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) };

	//CellData() { ZeroMemory(vPoints, 3 * sizeof(_float3)); }
	void CW()
	{
		Vec3 vA(vPoints[0].x, 0.f, vPoints[0].z);
		Vec3 vB(vPoints[1].x, 0.f, vPoints[1].z);
		Vec3 vC(vPoints[2].x, 0.f, vPoints[2].z);

		Vec3 vAB = Vec3(vB - vA);
		Vec3 vBC = Vec3(vC - vB);
		Vec3 vResult;
		vAB.Cross(vBC, vResult);
		if (vResult.y < 0.f)
		{
			_float3 vTemp = vPoints[1];
			vPoints[1] = vPoints[2];
			vPoints[2] = vTemp;
		}
	}
};

CNavMeshView::CNavMeshView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CNavMeshView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

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
	m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	if (!m_vecCells.empty())
	{
		for (_int i = 0; i < m_vecCells.size(); ++i)
		{
			_float3 vP0 = m_vecCells[i]->vPoints[0] + Vec3(0.f, 0.2f, 0.f);
			_float3 vP1 = m_vecCells[i]->vPoints[1] + Vec3(0.f, 0.2f, 0.f);
			_float3 vP2 = m_vecCells[i]->vPoints[2] + Vec3(0.f, 0.2f, 0.f);

			m_pBatch->Begin();
			DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Cyan);
			m_pBatch->End();
		}		
	}	

	return S_OK;
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

	for (auto iter : vecObjects)
	{
		CModel* pModel = iter->GetModel();

		if (nullptr != pModel)
		{
			vector<Vec3>& vecSurfaceVtx = pModel->GetSurfaceVtx();
			vector<FACEINDICES32>& vecSurfaceIdx = pModel->GetSurfaceIdx();
		
			for (auto idx : vecSurfaceIdx)
			{
				Vec3 v0 = Vec3::Transform(vecSurfaceVtx[idx._0], iter->GetTransform()->WorldMatrix());
				Vec3 v1 = Vec3::Transform(vecSurfaceVtx[idx._1], iter->GetTransform()->WorldMatrix());
				Vec3 v2 = Vec3::Transform(vecSurfaceVtx[idx._2], iter->GetTransform()->WorldMatrix());

				Vec3 v10 = v1 - v0;
				Vec3 v21 = v2 - v1;
				Vec3 vResult;
				v10.Cross(v21, vResult);

				if (0 > vResult.y)
				{
					Vec3 vTemp = v1;
					v1 = v2;
					v2 = vTemp;
				}

				vResult.Normalize();

				Vec3 vFloor(vResult.x, 0.0f, vResult.z);
				vFloor.Normalize();

				if (cosf(XMConvertToRadians(45.0f)) >= vResult.Dot(vFloor))
				{
					CellData* tCellData = new CellData;
					tCellData->vPoints[0] = v0;
					tCellData->vPoints[1] = v1;
					tCellData->vPoints[2] = v2;

					m_vecCells.push_back(tCellData);
				}
			}
		}
		else
		{
			continue;
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


	if (2 == m_vecPoints.size())
	{
		VertexPositionColor verts[2];

		_float3 vP0 = m_vecPoints[0] + Vec3(0.f, 0.2f, 0.f);
		_float3 vP1 = m_vecPoints[1] + Vec3(0.f, 0.2f, 0.f);

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
			_float3 vP0 = m_vecCells[i]->vPoints[0] + Vec3(0.f, 0.2f, 0.f);
			_float3 vP1 = m_vecCells[i]->vPoints[1] + Vec3(0.f, 0.2f, 0.f);
			_float3 vP2 = m_vecCells[i]->vPoints[2] + Vec3(0.f, 0.2f, 0.f);

			m_pBatch->Begin();
			DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Cyan);
			m_pBatch->End();
		}

		_float3 vP0 = m_vecCells[m_Item_Current]->vPoints[0] + Vec3(0.f, 0.2f, 0.f);
		_float3 vP1 = m_vecCells[m_Item_Current]->vPoints[1] + Vec3(0.f, 0.2f, 0.f);
		_float3 vP2 = m_vecCells[m_Item_Current]->vPoints[2] + Vec3(0.f, 0.2f, 0.f);

		m_pBatch->Begin();
		DX::DrawTriangle(m_pBatch, XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), XMLoadFloat3(&vP2), Colors::Coral);
		m_pBatch->End();
	}

	if (!m_vecSphere.empty())
	{
		for (auto& iter : m_vecSphere)
		{
			BoundingSphere tS(iter->Center + 0.2f * Vec3::UnitY, 0.5f);

			m_pBatch->Begin();
			DX::Draw(m_pBatch, tS, Colors::Cyan);
			m_pBatch->End();
		}
	}

	return S_OK;
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
	if (mapLayer.end() == iter) return false;
	vector<CGameObject*>& vecGroundObjects = iter->second->GetGameObjects();
	if (vecGroundObjects.empty()) return false;

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
		for (auto& iter : vecGroundObjects)
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

		if (pObject)
		{
			const Matrix& W = pObject->GetTransform()->WorldMatrix();
			pickPos = XMVector3TransformCoord(pickPos, W);
		}
		else
			return false;
	}

	m_vecPoints.push_back(pickPos);
	BoundingSphere* tSphere = new BoundingSphere;
	tSphere->Center = pickPos;
	tSphere->Radius = 1.f;
	m_vecSphere.push_back(tSphere);

	if (3 == m_vecPoints.size())
	{
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
		s2cPushBack(m_strPoints, to_string(pickPos.x) + " " + to_string(pickPos.y) + " " + to_string(pickPos.z));

	return true;
}

HRESULT CNavMeshView::Save()
{
	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/NavMesh/") + m_strFilePath + TEXT(".dat"), Write);

	for (auto& iter : m_vecCells)
		file->Write(*iter);

	return S_OK;
}

HRESULT CNavMeshView::Load()
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/NavMesh/") + m_strFilePath + TEXT(".dat"), Read);

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

CNavMeshView* CNavMeshView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavMeshView* pInstance = new CNavMeshView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
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