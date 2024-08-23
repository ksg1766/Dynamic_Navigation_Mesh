#include "stdafx.h"
#include "..\Public\Loader.h"

#include <filesystem>
#include "GameInstance.h"
#include "BackGround.h"
//
#include "BasicTerrain.h"
#include "AgentController.h"
#include "MainCameraController.h"
#include "FlyingCamera.h"
#include "MainCamera.h"
#include "StaticBase.h"
#include "Agent.h"
#include "AIAgent.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY ThreadEntry(void* pArg)
{
	/* DX가 사용하는 모든 컴 객체를 초기화한다.  */	
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;

	/* 새로운 스레드를 만들자. */
	/* 스레드 : 내 코드를 읽어주는 하나의 흐름? */
	/* 3 : 생성한 스레드가 호출해야하는 함수의 주소 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_GAMETOOL:
		hr = Loading_For_Level_GameTool();
		break;
	}

	if (FAILED(hr))
		return -1;	

	LeaveCriticalSection(&m_Critical_Section);

	return 0;	
}

HRESULT CLoader::Loading_For_Level_Logo()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_Logo();
	Loading_Scripts_For_Level_Logo();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");
	

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");
	Loading_GameObjects_For_Level_Logo();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_Logo()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Texture_BackGround */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TitleMenu.png")))))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_Logo()
{
	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_Logo()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_BackGround */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_GamePlay()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_GamePlay();
	Loading_Scripts_For_Level_GamePlay();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	Loading_GameObjects_For_Level_GamePlay();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BasicTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyingCamera"), CFlyingCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Agent */
  	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Agent"), CAgent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_AIAgent */
  	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AIAgent"), CAIAgent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Static */
	wstring strStaticFilePath = TEXT("../Bin/Resources/Models/Static/");
	for (const auto& entry : filesystem::directory_iterator(strStaticFilePath))
	{
		const wstring& strFileName = entry.path().stem();

		if (strFileName == TEXT("CStaticScene"))
			continue;

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_") + strFileName, CStaticBase::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_GameTool()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_GameTool();
	Loading_Scripts_For_Level_GameTool();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	Loading_GameObjects_For_Level_GameTool();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BasicTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyingCamera"), CFlyingCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Agent */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Agent"), CAgent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_AIAgent */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AIAgent"), CAIAgent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Static */
	wstring strStaticFilePath = TEXT("../Bin/Resources/Models/Static/");
	for (const auto& entry : filesystem::directory_iterator(strStaticFilePath))
	{
		const wstring& strFileName = entry.path().stem();

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_") + strFileName, CStaticBase::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);	

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}