#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(LAYERTAG::BACKGROUND)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->PlayBGM(TEXT("mus_mainmenu.ogg"), 0.3f)))
	//	__debugbreak();

	return S_OK;
}

HRESULT CLevel_Logo::Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Logo::LateTick(const _float& fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("�ΰ����Դϴ�."));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*if (KEY_DOWN_EX(KEY::ENTER))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else */if (KEY_PRESSING_EX(KEY::SHIFT) && KEY_DOWN_EX(KEY::E))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMETOOL))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const LAYERTAG& eLayerTag)
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*CGameObject* pGameObject = nullptr;

	pGameObject = pGameInstance->Add_GameObject(LEVEL_LOGO, LAYERTAG::BACKGROUND, TEXT("Prototype_GameObject_BackGround"));
	if (nullptr == pGameObject)	return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
 	__super::Free();


}
