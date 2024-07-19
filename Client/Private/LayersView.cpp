#include "stdafx.h"
#include "ViewMediator.h"
#include "LayersView.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Utils.h"
#include "Layer.h"
#include "DissolveManager.h"

CLayersView::CLayersView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CLayersView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

	return S_OK;
}

HRESULT CLayersView::Tick()
{
	Input();

	ImGui::Begin("Layers");

	InfoView();
	TreeGroups();

	ImGui::End();

	return S_OK;
}

HRESULT CLayersView::LateTick()
{

	return S_OK;
}

HRESULT CLayersView::DebugRender()
{

	return S_OK;
}

void CLayersView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		if (m_IsPickingActivated)
		{
			const POINT& p = m_pGameInstance->GetMousePos();
			m_pPrePickedObject = m_pCurPickedObject;
			m_pCurPickedObject = Pick(p.x, p.y);
		}
	}
}

CGameObject* CLayersView::Pick(_int screenX, _int screenY)
{
	Viewport& vp = m_pGameInstance->GetViewPort();

	_float fWidth = vp.width;
	_float fHeight = vp.height;

	Matrix projectionMatrix = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);

	_float viewX = (+2.0f * screenX / fWidth - 1.0f) / projectionMatrix(0, 0);
	_float viewY = (-2.0f * screenY / fHeight + 1.0f) / projectionMatrix(1, 1);

	Matrix matViewMatrixInv = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	CGameObject* pPicked = nullptr;
	_float fMinDistance = FLT_MAX;

	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	for (_uint iLayer = DEFAULT_LAYER_COUNT + 1; iLayer < (_uint)LAYERTAG::LAYER_END; ++iLayer)
	{
		LAYERTAG eLayer = static_cast<LAYERTAG>(iLayer);

		if (eLayer == LAYERTAG::DYNAMIC_LAYER_END ||
			eLayer == LAYERTAG::STATIC_LAYER_END ||
			eLayer == LAYERTAG::TERRAIN)
			continue;

		map<LAYERTAG, CLayer*>::iterator iter = mapLayers.find(eLayer);

		if (iter == mapLayers.end())
			continue;

		vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();

		for (auto& iter : vecObjects)
		{
			CSphereCollider* pSphere = iter->GetRigidBody()->GetSphereCollider();
			if (nullptr == pSphere)
				continue;

			Vec4 vRayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			Vec4 vRayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

			Vec3 vWorldRayOrigin = XMVector3TransformCoord(vRayOrigin, matViewMatrixInv);
			Vec3 vWorldRayDir = XMVector3TransformNormal(vRayDir, matViewMatrixInv);
			vWorldRayDir.Normalize();

			Ray cRay = Ray(vWorldRayOrigin, vWorldRayDir);

			_float fDistance = 0.f;
			if (false == pSphere->Intersects(cRay, OUT fDistance))
				continue;

			if (fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				pPicked = iter;
			}
		}
	}

	return pPicked;
}

void CLayersView::InfoView()
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

	if (m_pCurPickedObject && m_pCurPickedObject != m_pPrePickedObject)
	{
		ImGui::Text("Object Tag : %s", Utils::ToString(m_strPickedObject).data());

		m_pMediator->OnNotifiedSelected(m_pCurPickedObject);

		m_pCurPickedObject->GetShader()->SetPassIndex(1);
		if (m_pPrePickedObject && !m_pPrePickedObject->IsDead())
			m_pPrePickedObject->GetShader()->SetPassIndex(0);

		m_pPrePickedObject = m_pCurPickedObject;	// 이게 뭐지...?
	}
	else
	{
		if(m_pCurPickedObject)
			ImGui::Text("Object Tag : %s", Utils::ToString(m_strPickedObject).data());
		else
			ImGui::Text("Object Tag : %s", Utils::ToString(TEXT("null")).data());
	}

	ImGui::NewLine();

	if (ImGui::Button("Delete This Object"))
	{
		if (m_pCurPickedObject)	// TODO : DYING State 추가되면 예외처리 해줄 것.
		{
			m_pGameInstance->DeleteObject(m_pCurPickedObject);
			//CDissolveManager::GetInstance()->AddDissolve(m_pCurPickedObject);
			m_pCurPickedObject = m_pPrePickedObject;
		}
	}

	ImGui::NewLine();
}

void CLayersView::TreeGroups()
{
	if (ImGui::TreeNode("Default"))
	{
		TapGroups(0);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Dynamic"))
	{
		TapGroups(DEFAULT_LAYER_COUNT + 1);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Static"))
	{
		TapGroups(DEFAULT_LAYER_COUNT + 1 + DYNAMIC_LAYER_COUNT + 1);
		ImGui::TreePop();
	}
}

void CLayersView::TapGroups(_uint iIndex)
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar(LayerTag_string[iIndex], tab_bar_flags))
	{
		map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

		for (_uint iLayer = iIndex; iLayer < (_uint)LAYERTAG::LAYER_END; ++iLayer)
		{
			LAYERTAG eLayer = static_cast<LAYERTAG>(iLayer);

			if (eLayer == LAYERTAG::DEFAULT_LAYER_END ||
				eLayer == LAYERTAG::DYNAMIC_LAYER_END ||
				eLayer == LAYERTAG::STATIC_LAYER_END)
				break;

			if (eLayer == LAYERTAG::TERRAIN)	continue;

			if (ImGui::BeginTabItem(LayerTag_string[iLayer]))
			{
				map<LAYERTAG, class CLayer*>::iterator iter = mapLayers.find(eLayer);
				if (iter == mapLayers.end())
				{
					ImGui::EndTabItem();
					continue;
				}

				vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();
				vector<const _char*> vecItems;

				for (auto& iter : vecObjects)
				{
					s2cPushBack(vecItems, Utils::ToString(iter->GetObjectTag()));
				}

				if (m_Item_Current >= vecItems.size())
					m_Item_Current = vecItems.size() - 1;

				ImGui::ListBox(LayerTag_string[iLayer], &m_Item_Current, vecItems.data(), vecItems.size(), 7);
				
				if (0 <= m_Item_Current && !m_IsPickingActivated)
				{
					m_ePickedLayerTag = eLayer;
					m_strPickedObject = Utils::ToWString(vecItems[m_Item_Current]);
					m_pCurPickedObject = vecObjects[m_Item_Current];
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

CLayersView* CLayersView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLayersView* pInstance = new CLayersView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("Failed to Created : CLayersView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayersView::Free()
{
	Super::Free();
}
