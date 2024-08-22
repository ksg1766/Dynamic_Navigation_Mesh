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
