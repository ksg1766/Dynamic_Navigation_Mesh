#include "stdafx.h"
#include "..\Public\LavaTerrain.h"
#include "GameInstance.h"
#include "DebugTerrainGrid.h"

CLavaTerrain::CLavaTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: Super(pDevice, pContext)
{
}

CLavaTerrain::CLavaTerrain(const CLavaTerrain& rhs)
	: Super(rhs)
{
}

HRESULT CLavaTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLavaTerrain::Initialize(void* pArg)
{
	SetObjectTag(TEXT("LavaTerrain"));

	if (FAILED(Ready_FixedComponents(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLavaTerrain::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CLavaTerrain::LateTick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CLavaTerrain::DebugRender()
{

}

HRESULT CLavaTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetShader()->Begin();

	static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->Render();

#ifdef _DEBUG
	// DebugRender
	//Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CLavaTerrain::Ready_FixedComponents(void* pArg)
{
	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Shader */
	/*if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxNorTex"))))
		return E_FAIL;*/
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxDebug"))))
		return E_FAIL;

	GetShader()->SetPassIndex(3);

	/* Com_Terrain */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Terrain, TEXT("Prototype_Component_Terrain"))))
		return E_FAIL;

	if (FAILED(static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeJustGrid(1024, 1024)))
		return E_FAIL;
	/*if (FAILED(dynamic_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeWithHeightMap(TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))))
		return E_FAIL;*/

	/* Com_Texture*/
	/*CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(Super::AddComponent(pGameInstance->GetCurrentLevelIndex(), ComponentType::Texture, TEXT("Prototype_Component_Texture_Terrain"))))
		return E_FAIL;*/

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLavaTerrain::Ready_Scripts(void* pArg)
{
	/* Com_PlayerController */
	//if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Script, TEXT("Prototype_Component_DebugTerrainGrid"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLavaTerrain::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ))/* ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 0))*/)
	{
		return E_FAIL;
	}

	return S_OK;
}

CLavaTerrain* CLavaTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLavaTerrain* pInstance = new CLavaTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLavaTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLavaTerrain* CLavaTerrain::Clone(void* pArg)
{
	CLavaTerrain* pInstance = new CLavaTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLavaTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLavaTerrain::Free()
{
	Super::Free();
}
