#include "stdafx.h"
#include "..\Public\MainApp.h"
#include <filesystem>

#include "GameInstance.h"
#include "DissolveManager.h"
#include "Level_Loading.h"
#include "FlyingCameraController.h"
#include "DebugTerrainGrid.h"

CMainApp::CMainApp()	
	: m_pGameInstance(CGameInstance::GetInstance())
{	
	// D3D11_SAMPLER_DESC

	Safe_AddRef(m_pGameInstance);

	/*
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENBALE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	*/

	/* 
	_float4		vSrcColor(내가 백버퍼에 그릴려고하는 픽셀의 색), vDestColor(이미 백버퍼에 그려져있던 색);

	_float4		vResult =
		(vSrcColor * vSrcColor.a) + (vDestColor * (1.f - vSrcColor.a));
		*/
}


HRESULT CMainApp::Initialize()
{
	/* 1. 내 게임의 초기화를 수행. */
	/* 1-1. 그래픽장치를 초기화한다. */
	/* 1-2. 사운드장치를 초기화한다. */
	/* 1-3. 입력장치를 초기화한다. */
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

	/* 1-4. 게임내에서 사용할 레벨(씬)을 생성.   */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
	/* 게임내에 존재하는 여러 객체들의 갱신. */
	/* 레벨의 갱신 */
	m_pGameInstance->Tick(fTimeDelta);
	
	// Temp
	CDissolveManager::GetInstance()->Tick_Dissolve(fTimeDelta);
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

	/* 게임내에 존재하는 여러 객체들의 렌더링. */
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.9f, 0.9f, 0.9f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
#ifdef _DEBUG
	//if(LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	//	m_pGameInstance->Render_QuadTree();
#endif
	m_pRenderer->Draw_RenderObjects();

	m_pGameInstance->DebugRender();
	/* 초기화한 장면에 객체들을 그린다. */
	m_pGameInstance->Present();

	m_pGameInstance->FinalTick();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 로고레베릉ㄹ 할당하고 싶었지만. 로고레벨을 위한 로딩레벨을 먼저 생성하여 로딩작업을 수행할꺼야. */
	/* 로딩객체에게 eLevelID라는 내가 실제 할당ㅎ아고 싶었던 레벨열거체를 준거지?! 실제할당하고싶었던 레벨에 자원을 준비라하라고 */
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Reserve_Client_Managers()
{
	if (FAILED(CDissolveManager::GetInstance()->Reserve_Manager(m_pDevice)))
		return E_FAIL;

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

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTexFetchAnim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexFetchAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexFetchAnim.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxNonAnimInstancing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnimInstancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnimInstancing.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimInstancing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimInstancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimInstancing.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
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

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Grid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Grid"),
		CVIBuffer_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_Component_VIBuffer_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Dot"),
		CVIBuffer_Dot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Model */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model"),
		CModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* For.Prototype_Component_RigidStatic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidStatic"),
		CRigidStatic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_RigidDynamic */
	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidDynamic"),
		CRigidDynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 일단 여기에 넣어보자
	/* For.Prototype_Component_SphereCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_SphereCollider"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OBBCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_OBBCollider"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OBBCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_SkyBox */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/SkyBox1.dds")))))
		return E_FAIL;*/
	
	/* For.Prototype_Component_Texture_Terrain_Alpha */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Alpha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_Alpha_%d.png"), 8))))
		return E_FAIL;*/

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
				//
				continue;
				//

				XMStoreFloat4x4(&matPivot, XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationY(XMConvertToRadians(90.0f)));

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_" + strFileName),
					CModel::Create(m_pDevice, m_pContext, strStaticFilePath + strFileName, desc, matPivot))))
					return E_FAIL;
			}
			else
			{
				if (TEXT("Building_a") != strFileName)
				{
				//	continue;
				}

				XMStoreFloat4x4(&matPivot, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f)));

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

	/* For.Prototype_Component_DebugTerrainGrid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_DebugTerrainGrid"),
		CDebugTerrainGrid::Create(m_pDevice, m_pContext))))
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

	CDissolveManager::GetInstance()->Free();

	CGameInstance::Release_Engine();
}
