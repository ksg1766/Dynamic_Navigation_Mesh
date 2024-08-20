#include "stdafx.h"
#include "PrefabsView.h"
#include "ViewMediator.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Terrain.h"
#include <filesystem>
#include "Utils.h"

CPrefabsView::CPrefabsView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CPrefabsView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

	m_pTerrainBuffer = reinterpret_cast<CTerrain*>(pArg);
	m_vPickedPosition = Vec3::Zero;

	return S_OK;
}

HRESULT CPrefabsView::Tick()
{
	Input();

	ImGui::Begin("Prefabs");

	InfoView();
	TapGroups();

	ImGui::End();

	return S_OK;
}

HRESULT CPrefabsView::LateTick()
{

	return S_OK;
}

HRESULT CPrefabsView::DebugRender()
{

	return S_OK;
}

HRESULT CPrefabsView::PlaceObstacle(const wstring& strObjectTag, const Matrix& matWorld, OUT CGameObject*& pGameObjectRet)
{
	const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + strObjectTag;

	pGameObjectRet = m_pGameInstance->CreateObject(strPrototypeTag, LAYERTAG::WALL);

	if (nullptr == pGameObjectRet)
	{
		return E_FAIL;
	}

	pGameObjectRet->GetTransform()->Set_WorldMatrix(matWorld);

	return S_OK;
}

void CPrefabsView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	_float fPickDistance;

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		if (m_IsPickingActivated)
		{
			const POINT& p = m_pGameInstance->GetMousePos();
			if (m_pTerrainBuffer->Pick(p.x, p.y, m_vPickedPosition, fPickDistance, m_pTerrainBuffer->GetTransform()->WorldMatrix()))
			{
				PlaceObject(m_ePickedLayerTag, m_strPickedObject, m_vPickedPosition);
			}
		}
	}
}

void CPrefabsView::InfoView()
{
	ImGui::Text("This window has some useful function for Map Tool.");
	ImGui::NewLine();

	const POINT& p = m_pGameInstance->GetMousePos();
	ImGui::Text("Mouse Viewport Position : %d, %d", p.x, p.y);
	ImGui::NewLine();

	ImGui::Text("Picked Position : %.3f, %.3f, %.3f", m_vPickedPosition.x, m_vPickedPosition.y, m_vPickedPosition.z);
	ImGui::NewLine();

	ImGui::Text("Picking ");
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
}

void CPrefabsView::TapGroups()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tool", tab_bar_flags))
	{
		/*if (ImGui::BeginTabItem("Ground"))
		{
			static const _char* items[] =
			{
				"Barrel", "BistroExterior", "BistroInterior", "BistroInterior_Wine", "Bucket", "Building_a", "Building_b", "Building_c", "Building_d", "Building_f", "Cart_a", "Cart_Wheel_a", "Crate",
				"Fence_a", "Fence_b", "Fence_c", "Grass", "Lantern_post", "Logs_a", "Mushroom_a", "Mushroom_b", "Mushroom_c", "Mushroom_d",
				"Rock_a", "Rock_c", "Rock_d", "Sack_a", "Sewers_entrance", "Sign_a", "Sign_b", "Sign_c", "Tent", "Well"	
				"Dumpster", "Receptacle_Recycling", "Light_Lamppost", "Picnic_Table", "Receptacle_Recycling", "Signal_Separated", "Signal_Separated_No_Stoplight"
			};

			if (m_Item_Current >= IM_ARRAYSIZE(items))
				m_Item_Current = IM_ARRAYSIZE(items) - 1;

			ImGui::ListBox("Ground Objects", &m_Item_Current, items, IM_ARRAYSIZE(items), 10);
			m_ePickedLayerTag = LAYERTAG::GROUND;
			m_strPickedObject = Utils::ToWString(items[m_Item_Current]);
			ImGui::EndTabItem();
		}*/
		if (ImGui::BeginTabItem("Obstacle"))
		{
			static const _char* items[] =
			{
				/*"Building_Kwow", "Building_Sheleg", "Building_ToyHotel", "bus1", "bus2", "bus3", "bus4", "bus5",
				"dumpster2", "dumpster3", "dumpster4", "dumpster5", "EmeraldSquare_Base", "European_Linden1", "European_Linden2",
				"Picnic_table1", "Picnic_table2", "Picnic_table3", "Picnic_table4", "Picnic_table5", "Picnic_table6", "Picnic_table7", "Picnic_table8",
				"Red_Maple_Young1", "Red_Maple_Young2", "White_Oak_13",


				"Barrel", "Bucket", "Building_a", "Building_b", "Building_c", "Building_d", "Building_f", "Cart_a", "Cart_Wheel_a", "Crate",
				"Fence_a", "Fence_b", "Fence_c", "Grass", "Lantern_post", "Logs_a", "Mushroom_a", "Mushroom_b", "Mushroom_c", "Mushroom_d",
				"Rock_a", "Rock_c", "Rock_d", "Sack_a", "Sewers_entrance", "Sign_a", "Sign_b", "Sign_c", "Tent", "Well"*/

				"Dumpster", "Receptacle_Recycling", "Light_Lamppost", "Picnic_Table", "Receptacle_Recycling", "Signal_Separated", "Signal_Separated_No_Stoplight"
			};

			if (m_Item_Current >= IM_ARRAYSIZE(items))
				m_Item_Current = IM_ARRAYSIZE(items) - 1;

			ImGui::ListBox("Obstacle Objects", &m_Item_Current, items, IM_ARRAYSIZE(items), 10);
			m_ePickedLayerTag = LAYERTAG::WALL;
			m_strPickedObject = Utils::ToWString(items[m_Item_Current]);
			
			ImGui::EndTabItem();
		}
		/*if (ImGui::BeginTabItem("IgnoreCollision"))
		{
			static const _char* items[] =
			{
				"empty"
			};

			if (m_Item_Current >= IM_ARRAYSIZE(items))
				m_Item_Current = IM_ARRAYSIZE(items) - 1;

			ImGui::ListBox("Else Objects", &m_Item_Current, items, IM_ARRAYSIZE(items), 10);
			m_ePickedLayerTag = LAYERTAG::IGNORECOLLISION;
			m_strPickedObject = Utils::ToWString(items[m_Item_Current]);
			
			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();
	}
}

void CPrefabsView::PlaceObject(const LAYERTAG& eLayerTag, const wstring& strObjectTag, const Vec3& vPickPosition, _bool bNotify)
{
	if (eLayerTag == LAYERTAG::LAYER_END) return;

	if (true == bNotify && eLayerTag == LAYERTAG::WALL)
	{
		m_pMediator->OnNotifiedPlaceObstacle(strObjectTag, vPickPosition);
	}
	else
	{
		const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + strObjectTag;
		CGameObject* pGameObject = m_pGameInstance->CreateObject(strPrototypeTag, eLayerTag);
		pGameObject->GetTransform()->Translate(vPickPosition);
	}
}

CPrefabsView* CPrefabsView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTerrain* m_pTerrainBf)
{
	CPrefabsView* pInstance = new CPrefabsView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(m_pTerrainBf)))
	{
		MSG_BOX("Failed to Created : CPrefabsView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrefabsView::Free()
{
	Super::Free();
}
