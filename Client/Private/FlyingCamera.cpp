#include "stdafx.h"
#include "..\Public\FlyingCamera.h"
#include "GameInstance.h"

CFlyingCamera::CFlyingCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFlyingCamera::CFlyingCamera(const CFlyingCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlyingCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyingCamera::Initialize(void * pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	

	return S_OK;
}

void CFlyingCamera::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);


}

void CFlyingCamera::LateTick(_float fTimeDelta)
{
	Super::LateTick(fTimeDelta);

}

HRESULT CFlyingCamera::Ready_FixedComponents()
{
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;
	
	/* Com_Camera */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Camera, TEXT("Prototype_Component_Camera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlyingCamera::Ready_Scripts()
{
	/* Com_PlayerController */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Script, TEXT("Prototype_Component_FlyingCameraController"))))
		return E_FAIL;

	return S_OK;
}

CFlyingCamera* CFlyingCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlyingCamera*		pInstance = new CFlyingCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFlyingCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlyingCamera::Clone(void * pArg)
{
	CFlyingCamera*		pInstance = new CFlyingCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFlyingCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyingCamera::Free()
{
	Super::Free();
}
