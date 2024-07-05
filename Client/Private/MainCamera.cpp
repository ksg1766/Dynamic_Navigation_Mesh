#include "stdafx.h"
#include "..\Public\MainCamera.h"
#include "GameInstance.h"
#include "MainCameraController.h"

CMainCamera::CMainCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCamera::Initialize(void * pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	

	return S_OK;
}

void CMainCamera::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);


}

void CMainCamera::LateTick(_float fTimeDelta)
{
	Super::LateTick(fTimeDelta);

}

HRESULT CMainCamera::Ready_FixedComponents()
{
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;
	
	/* Com_Camera */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Camera, TEXT("Prototype_Component_Camera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::Ready_Scripts()
{
	/* Com_MainCameraController */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MainCameraController"))))
		return E_FAIL;

	m_pController = static_cast<CMainCameraController*>(m_vecScripts[0]);

	return S_OK;
}

CMainCamera* CMainCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMainCamera*		pInstance = new CMainCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCamera::Clone(void * pArg)
{
	CMainCamera*		pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainCamera::Free()
{
	Super::Free();
}
