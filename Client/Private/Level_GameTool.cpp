#include "stdafx.h"
#include "..\Public\Level_GameTool.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "BasicTerrain.h"
#include "Terrain.h"

#include "AnimationView.h"
#include "PrefabsView.h"
#include "LayersView.h"
#include "TransformView.h"
#include "SaveLoadView.h"
#include "NavMeshView.h"

#include "ViewMediator.h"

CLevel_GameTool::CLevel_GameTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GameTool::Initialize()
{
	m_pGameInstance = GET_INSTANCE(CGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	
	if (FAILED(Ready_Lights()))
		return E_FAIL;

 	if (FAILED(Ready_Layer_Default()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Terrain()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Ground()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Wall()))
		return E_FAIL;

	if (FAILED(Ready_Tools()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GameTool::Tick(const _float& fTimeDelta)
{
	//m_pGameInstance->Update_QuadTree();

	// UI
	if (!m_IsImGUIReady)
	{
		m_IsImGUIReady = true;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	

	//
	//if (m_pPrefabsView)
	//	m_pPrefabsView->Tick();
	//if (m_pLayersView)
	//	m_pLayersView->Tick();
	//if (m_pTransformView)
	//	m_pTransformView->Tick();
	/*if (m_pSaveLoadView)
		m_pSaveLoadView->Tick();
	if (m_pAnimationView)
		m_pAnimationView->Tick();*/
	if (m_pNavMeshView)
		m_pNavMeshView->Tick();

	//

	//ImGUIDemo();


	return S_OK;
}

HRESULT CLevel_GameTool::LateTick(const _float& fTimeDelta)
{
	//SetWindowText(g_hWnd, TEXT("게임 툴 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GameTool::DebugRender()
{
	if (m_IsImGUIReady)
	{
		//m_pPrefabsView->DebugRender();
		//m_pLayersView->DebugRender();
		//m_pTransformView->DebugRender();
		//m_pSaveLoadView->DebugRender();
		//m_pAnimationView->DebugRender();
		m_pNavMeshView->DebugRender();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Lights()
{
	LIGHT_DESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Layer_Default()
{
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::DEFAULT;

	/*pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMETOOL, eLayerTag, TEXT("Prototype_GameObject_SkyBox"));
	if (nullptr == pGameObject)	return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Layer_Terrain()
{
	//wstring strPath = TEXT("../Bin/Resources/Textures/Terrain/cityterrain3.bmp");
	//wstring strPath = TEXT("../Bin/Resources/Textures/Terrain/testmaze0.bmp");
	wstring strPath = TEXT("../Bin/Resources/Textures/Terrain/Terrain0.bmp");
	//wstring strPath = TEXT("../Bin/Resources/Textures/Terrain/TerrainBig0.bmp");
	m_pBasicTerrain = dynamic_cast<CBasicTerrain*>(m_pGameInstance->Add_GameObject(
		LEVEL_GAMETOOL,
		LAYERTAG::TERRAIN,
		TEXT("Prototype_GameObject_BasicTerrain"),
		&strPath));
	if (nullptr == m_pBasicTerrain) return E_FAIL;
	/*m_pBasicTerrain = dynamic_cast<CBasicTerrain*>(m_pGameInstance->Add_GameObject(
		LEVEL_GAMETOOL,
		LAYERTAG::TERRAIN,
		TEXT("Prototype_GameObject_BasicTerrain")));
	if (nullptr == m_pBasicTerrain) return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Layer_Ground()
{
	CGameObject* pGameObject = nullptr;

	LAYERTAG	eLayerTag = LAYERTAG::GROUND;

	/*wstring strPath = TEXT("../Bin/Resources/Textures/Terrain/Terrain_Alpha_6.bmp");
	m_pBasicTerrain = dynamic_cast<CBasicTerrain*>(m_pGameInstance->Add_GameObject(LEVEL_GAMETOOL, eLayerTag, TEXT("Prototype_GameObject_BasicTerrain"), &strPath));
	if (nullptr == m_pBasicTerrain) return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Layer_Camera()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMETOOL, LAYERTAG::CAMERA, TEXT("Prototype_GameObject_FlyingCamera"));
	if (nullptr == pGameObject)	return E_FAIL;
	pGameObject->GetTransform()->Translate(Vec3(0.0f, 500.0f, 0.0f));

	m_pGameInstance->ChangeCamera(TEXT("FlyingCamera"));

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Layer_Wall()
{
	CGameObject* pGameObject = nullptr;

	return S_OK;
}

HRESULT CLevel_GameTool::Ready_Tools()
{
	m_pMediator = new CViewMediator;

	m_pPrefabsView = CPrefabsView::Create(m_pDevice, m_pContext, dynamic_cast<CTerrain*>(m_pBasicTerrain->GetFixedComponent(ComponentType::Terrain)));
	m_pMediator->SetPrefabsView(m_pPrefabsView);	// TODO:나중에 Initialize에서 하게 하든지 하자...

	m_pLayersView = CLayersView::Create(m_pDevice, m_pContext);
	m_pMediator->SetLayersView(m_pLayersView);
	
	m_pTransformView = CTransformView::Create(m_pDevice, m_pContext);
	m_pMediator->SetTransformView(m_pTransformView);

	m_pSaveLoadView = CSaveLoadView::Create(m_pDevice, m_pContext);
	m_pMediator->SetSaveLoadView(m_pSaveLoadView);

	m_pAnimationView = CAnimationView::Create(m_pDevice, m_pContext);
	m_pMediator->SetAnimationView(m_pAnimationView);
	
	m_pNavMeshView = CNavMeshView::Create(m_pDevice, m_pContext, dynamic_cast<CTerrain*>(m_pBasicTerrain->GetFixedComponent(ComponentType::Terrain)));
	m_pMediator->SetNavMeshView(m_pNavMeshView);

	return S_OK;
}

void CLevel_GameTool::ImGUIDemo()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

CLevel_GameTool* CLevel_GameTool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GameTool*	pInstance = new CLevel_GameTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GameTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GameTool::Free()
{
	Safe_Release(m_pPrefabsView);
	Safe_Release(m_pLayersView);
	Safe_Release(m_pTransformView);
	Safe_Release(m_pSaveLoadView);
	Safe_Release(m_pAnimationView);
	Safe_Release(m_pMediator);
	Safe_Release(m_pBasicTerrain);

	Super::Free();

	RELEASE_INSTANCE(CGameInstance);
}
