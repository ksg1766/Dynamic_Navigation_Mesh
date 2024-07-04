#include "stdafx.h"
#include "SaveLoadView.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Utils.h"
#include <filesystem>
#include "Layer.h"
#include "FileUtils.h"

CSaveLoadView::CSaveLoadView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CSaveLoadView::Initialize(void* pArg)
{
	Super::Initialize(pArg);
	m_arrLayerTable[static_cast<_uint>(LAYERTAG::GROUND)] = true;
	m_arrLayerTable[static_cast<_uint>(LAYERTAG::WALL)] = true;

	return S_OK;
}

HRESULT CSaveLoadView::Tick()
{
	Input();

	ImGui::Begin("DataFiles");

	InfoView();
	TreeGroups();

	ImGui::End();

	return S_OK;
}

HRESULT CSaveLoadView::LateTick()
{

	return S_OK;
}

HRESULT CSaveLoadView::DebugRender()
{

	return S_OK;
}

void CSaveLoadView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;


}

HRESULT CSaveLoadView::Save()
{
	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	FileUtils FileUtils;
	FileUtils.Open(TEXT("../Bin/LevelData/") + m_strFilePath, Write);

    for (_uint iLayer = 0; iLayer < static_cast<_uint>(LAYERTAG::LAYER_END); ++iLayer)
    {
		LAYERTAG eLayer = static_cast<LAYERTAG>(iLayer);

		/*if (eLayer == LAYERTAG::DEFAULT_LAYER_END || eLayer == LAYERTAG::DYNAMIC_LAYER_END ||
			eLayer == LAYERTAG::STATIC_LAYER_END || eLayer == LAYERTAG::CAMERA || eLayer == LAYERTAG::TERRAIN)
			continue;*/

		if (!m_arrLayerTable[static_cast<_uint>(eLayer)])
			continue;

		map<LAYERTAG, CLayer*>::iterator iterLayer = mapLayers.find(eLayer);
		if (iterLayer == mapLayers.end())
			continue;

        vector<CGameObject*>& vecObjList = iterLayer->second->GetGameObjects();
        for (auto& iterObj : vecObjList)
        {
			const Matrix& matWorld = iterObj->GetTransform()->WorldMatrix();
			const wstring& strObjectTag = iterObj->GetObjectTag();

			string strLayer = LayerTag_string[static_cast<_int>(eLayer)];
			FileUtils.Write(strLayer);
			FileUtils.Write(matWorld);
			FileUtils.Write(Utils::ToString(strObjectTag));
        }        
    }

    return S_OK;
}

HRESULT CSaveLoadView::Load()
{
	FileUtils FileUtils;
	FileUtils.Open(TEXT("../Bin/LevelData/") + m_strFilePath, Read);

    while (true)
    {
        // key °ª ÀúÀå
		LAYERTAG eLayer = LAYERTAG::LAYER_END;
		string strLayerTag = "";

        if (false == FileUtils.Read(strLayerTag))
            break;

		const _int LayerTag_End = sizeof(LayerTag_string) / sizeof(_char*);
		for (_int i = 0; i < LayerTag_End; ++i)
		{
			if (0 == strcmp(LayerTag_string[i], strLayerTag.c_str()))
			{
				eLayer = static_cast<LAYERTAG>(i);
				break;
			}
		}
		if (LAYERTAG::LAYER_END == eLayer)
			__debugbreak();

		Matrix matWorld = FileUtils.Read<Matrix>();
		string tempObjectTag;
		FileUtils.Read(tempObjectTag);

		if (!m_arrLayerTable[static_cast<_uint>(eLayer)])
			continue;

		if (static_cast<_uint>(LAYERTAG::DEFAULT_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer))
		{

		}

		if (static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::STATIC_LAYER_END) > static_cast<_uint>(eLayer))
		{
			const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + Utils::ToWString(tempObjectTag);

			CGameObject* pGameObject = m_pGameInstance->CreateObject(strPrototypeTag, eLayer);
			if (nullptr == pGameObject)
				__debugbreak();
			pGameObject->GetTransform()->Set_WorldMatrix(matWorld);
		}
    }

    return S_OK;
}

HRESULT CSaveLoadView::Clear()
{
	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();

	for (_uint iLayer = 0; iLayer < static_cast<_uint>(LAYERTAG::LAYER_END); ++iLayer)
	{
		LAYERTAG eLayer = static_cast<LAYERTAG>(iLayer);

		if (/*eLayer == LAYERTAG::DEFAULT_LAYER_END || eLayer == LAYERTAG::DYNAMIC_LAYER_END ||
			eLayer == LAYERTAG::STATIC_LAYER_END || */eLayer == LAYERTAG::DEFAULT || eLayer == LAYERTAG::CAMERA || eLayer == LAYERTAG::TERRAIN)
			continue;

		map<LAYERTAG, CLayer*>::iterator iterLayer = mapLayers.find(eLayer);
		if (iterLayer == mapLayers.end())
			continue;

		vector<CGameObject*>& refObjects = iterLayer->second->GetGameObjects();

		for_each(refObjects.begin(), refObjects.end(), [&](CGameObject* pObj) { m_pGameInstance->DeleteObject(pObj); });
		refObjects.clear();
	}

	return S_OK;
}

void CSaveLoadView::InfoView()
{
	ImGui::Text("This window has some functions To Save/Load Level Data.");
	ImGui::NewLine();
	if (ImGui::Button("Save New"))	{ m_bSaveNewButton = !m_bSaveNewButton; }
	if (m_bSaveNewButton)
	{
		ImGui::NewLine();
		const _char* hint = "Enter the Name of New file";
		
		ImGui::InputTextWithHint("NewFileName", hint, m_szFileNameBuf, IM_ARRAYSIZE(m_szFileNameBuf));

		if (ImGui::Button("Make & Save"))
		{
			strcat_s(m_szFileNameBuf, sizeof(m_szFileNameBuf), ".dat");

			m_strFilePath = Utils::ToWString(string(m_szFileNameBuf));
			Save();
		}
	}
	else
	{
		/*ImGui::SameLine();
		if (ImGui::Button("Save Override"))
		{
			Save();
		}*/
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			Load();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			Clear();
		}
	}
	ImGui::NewLine();

	vector<const _char*> vecItems;
	wstring strDirectory = TEXT("../Bin/LevelData/");

	if (filesystem::exists(strDirectory) && filesystem::is_directory(strDirectory))
	{
		for (const auto& entry : filesystem::directory_iterator(strDirectory))
		{
			if (entry.is_regular_file())
			{
				s2cPushBack(vecItems, Utils::ToString(entry.path().filename()));
			}
		}
	}

	ImGui::ListBox("Data Files", &m_Item_Current, vecItems.data(), vecItems.size(), 5);

	if (vecItems.size())
	{
		m_strFilePath = Utils::ToWString(vecItems[m_Item_Current]);
	}
}

void CSaveLoadView::TreeGroups()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::TreeNode("Dynamic"))
	{
		_uint iIndexOffset = DEFAULT_LAYER_COUNT + 1;
		for (_uint i = 0; i < DYNAMIC_LAYER_COUNT; ++i)
			ImGui::Checkbox(LayerTag_string[iIndexOffset + i], &m_arrLayerTable[iIndexOffset + i]);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Static"))
	{
		_uint iIndexOffset = DEFAULT_LAYER_COUNT + 1 + DYNAMIC_LAYER_COUNT + 1;
		for (_uint i = 0; i < STATIC_LAYER_COUNT; ++i)
		{
			if (i == static_cast<_uint>(LAYERTAG::TERRAIN))
				continue;
			ImGui::Checkbox(LayerTag_string[iIndexOffset + i], &m_arrLayerTable[iIndexOffset + i]);
		}

		ImGui::TreePop();
	}
}

CSaveLoadView* CSaveLoadView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSaveLoadView* pInstance = new CSaveLoadView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("Failed to Created : CSaveLoadView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSaveLoadView::Free()
{
	Super::Free();
}
