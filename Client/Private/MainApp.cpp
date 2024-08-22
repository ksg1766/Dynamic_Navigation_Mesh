#include "stdafx.h"
#include "..\Public\MainApp.h"
#include <filesystem>

#include "GameInstance.h"
#include "Level_Loading.h"
#include "FlyingCameraController.h"
#include "MainCameraController.h"
#include "DebugTerrainGrid.h"
#include "AgentController.h"
#include "AIController.h"

CMainApp::CMainApp()	
	: m_pGameInstance(CGameInstance::GetInstance())
{	
	Safe_AddRef(m_pGameInstance);
}


HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext, g_hInstance)))
		return E_FAIL;

	if (FAILED(Reserve_Client_Managers()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Components()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Scripts()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	++m_iFps;

	if (m_dwTime + 1000 < GetTickCount())
	{
#ifdef _DEBUG
		swprintf_s(m_szFPS, L"FPS : %d - DEBUG", m_iFps);
#else
		swprintf_s(m_szFPS, L"FPS : %d - RELEASE", m_iFps);
#endif
		SetWindowText(g_hWnd, m_szFPS);

		m_iFps = 0;
		m_dwTime = GetTickCount();
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.9f, 0.9f, 0.9f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
#ifdef _DEBUG
	//if(LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	//	m_pGameInstance->Render_QuadTree();
#endif
	m_pRenderer->Draw_RenderObjects();

	m_pGameInstance->DebugRender();

	m_pGameInstance->Present();

	m_pGameInstance->FinalTick();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Reserve_Client_Managers()
{
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), 
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_TriangleTest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TriangleTest"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_TriangleTest.hlsl"), nullptr, 0))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMeshInstancing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshInstancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstancing.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeNom*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeNom"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeNom.hlsl"), VTXCUBENOM::Elements, VTXCUBENOM::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxDebug*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxDebug"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxDebug.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Grid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Grid"),
		CVIBuffer_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OBBCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FlatBlue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Colors/FlatBlue.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FlatMagenta"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Colors/FlatMagenta.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FlatNormal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Colors/FlatNormal.png")))))
		return E_FAIL;

	/* For.Prototype_Component_NavMeshAgent */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_NavMeshAgent"),
		CNavMeshAgent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Static */
	{
		wstring strStaticFilePath = TEXT("../Bin/Resources/Models/Static/");
		for (const auto& entry : filesystem::directory_iterator(strStaticFilePath))
		{
			//Matrix matPivot = Matrix::Identity;
			const wstring& strFileName = entry.path().stem();
			//SOCKETDESC desc = SOCKETDESC();

			SOCKETDESC desc = SOCKETDESC();
			Matrix matPivot = Matrix::Identity;

			if (TEXT("EmeraldSquare_Day") == strFileName)
			{
				continue;
			}
			//else if (TEXT("EmeraldSquare_Base") == strFileName)
			else if (TEXT("Building_Kwow") == strFileName ||
					TEXT("Building_Sheleg") == strFileName ||
					TEXT("Building_ToyHotel") == strFileName ||
					TEXT("bus1") == strFileName ||
					TEXT("bus2") == strFileName ||
					TEXT("bus3") == strFileName ||
					TEXT("bus4") == strFileName ||
					TEXT("bus5") == strFileName ||
					TEXT("dumpster2") == strFileName ||
					TEXT("dumpster3") == strFileName ||
					TEXT("dumpster4") == strFileName ||
					TEXT("dumpster5") == strFileName ||
					TEXT("EmeraldSquare_Base") == strFileName ||
					TEXT("European_Linden1") == strFileName ||
					TEXT("European_Linden2") == strFileName ||
					TEXT("Picnic_table1") == strFileName ||
					TEXT("Picnic_table2") == strFileName ||
					TEXT("Picnic_table3") == strFileName ||
					TEXT("Picnic_table4") == strFileName ||
					TEXT("Picnic_table5") == strFileName ||
					TEXT("Picnic_table6") == strFileName ||
					TEXT("Picnic_table7") == strFileName ||
					TEXT("Picnic_table8") == strFileName ||
					TEXT("Red_Maple_Young1") == strFileName ||
					TEXT("Red_Maple_Young2") == strFileName ||
					TEXT("White_Oak_13") == strFileName ||
					TEXT("Dumpster") == strFileName ||
					TEXT("Receptacle_Recycling") == strFileName ||
					TEXT("Light_Lamppost") == strFileName ||
					TEXT("Picnic_Table") == strFileName ||
					TEXT("Bus") == strFileName ||
					TEXT("Signal_Separated") == strFileName ||
					TEXT("Signal_Separated_No_Stoplight") == strFileName)
			{
				//continue;

				XMStoreFloat4x4(&matPivot, XMMatrixRotationY(XMConvertToRadians(90.0f)));

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_" + strFileName),
					CModel::Create(m_pDevice, m_pContext, strStaticFilePath + strFileName, desc, matPivot))))
					return E_FAIL;
			}
			else if (TEXT("BistroExterior") == strFileName || TEXT("BistroInterior") == strFileName || TEXT("BistroInterior_Wine") == strFileName)
			{
				continue;
				XMStoreFloat4x4(&matPivot, XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationY(XMConvertToRadians(90.0f)));

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_" + strFileName),
					CModel::Create(m_pDevice, m_pContext, strStaticFilePath + strFileName, desc, matPivot))))
					return E_FAIL;
			}			
			else
			{
				//continue;
				//
				XMStoreFloat4x4(&matPivot, XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(90.0f)));

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_" + strFileName),
					CModel::Create(m_pDevice, m_pContext, strStaticFilePath + strFileName, desc, matPivot))))
					return E_FAIL;
			}
		}
	}

	/* For.Prototype_Component_Model_Skeletal */
	{
		wstring strSkeletalFilePath = TEXT("../Bin/Resources/Models/Skeletal/");
		for (const auto& entry : filesystem::directory_iterator(strSkeletalFilePath))
		{
			const wstring& strFileName = entry.path().stem();

			SOCKETDESC desc = SOCKETDESC();
			Matrix matPivot = Matrix::Identity;
			XMStoreFloat4x4(&matPivot, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f)));
			
			{
				//XMStoreFloat4x4(&matPivot, XMMatrixScaling(0.01f, 0.01f, 0.01f));
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_" + strFileName),
					CModel::Create(m_pDevice, m_pContext, strSkeletalFilePath + strFileName, desc, matPivot))))
					return E_FAIL;
			}
		}
	}

	Safe_AddRef(m_pRenderer);
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Scripts()
{
	/* For.Prototype_Component_FlyingCameraController */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FlyingCameraController"),
		CFlyingCameraController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_MainCameraController*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_MainCameraController"),
		CMainCameraController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_DebugTerrainGrid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_DebugTerrainGrid"),
		CDebugTerrainGrid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_AgentController*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_AgentController"),
		CAgentController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_AIController*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_AIController"),
		CAIController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{	
	__super::Free();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
