#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CBackGround::CBackGround(const CGameObject & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Tick(const _float& fTimeDelta)
{
}

void CBackGround::LateTick(const _float& fTimeDelta)
{

	GetRenderer()->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetShader()->Begin();

	GetBuffer()->Render();

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxPosTex"))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Rect"))))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::AddComponent(LEVEL_LOGO, ComponentType::Texture, TEXT("Prototype_Component_Texture_BackGround"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{
	_float4x4		WorldMatrix;
	_float4x4		IdentityMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	WorldMatrix._11 = 2.f;
	WorldMatrix._22 = 2.f;
	if (FAILED(GetShader()->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &IdentityMatrix)))
		return E_FAIL;
	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &IdentityMatrix)))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(GetTexture()->Bind_ShaderResources(GetShader(), "g_Textures")))
		return E_FAIL;

	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBackGround*	pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackGround::Clone(void* pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	Super::Free();
}