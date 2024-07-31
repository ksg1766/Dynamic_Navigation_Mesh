#include "stdafx.h"
#include "..\Public\BasicTerrain.h"
#include "GameInstance.h"
#include "DebugTerrainGrid.h"

CBasicTerrain::CBasicTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: Super(pDevice, pContext)
{
}

CBasicTerrain::CBasicTerrain(const CBasicTerrain& rhs)
	: Super(rhs)
{
}

HRESULT CBasicTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBasicTerrain::Initialize(void* pArg)
{
	SetObjectTag(TEXT("BasicTerrain"));

	if (FAILED(Ready_FixedComponents(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBasicTerrain::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CBasicTerrain::LateTick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);
	//GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	//GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CBasicTerrain::DebugRender()
{

}

HRESULT CBasicTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetShader()->Begin();

	static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->Render();

#ifdef _DEBUG
	// DebugRender
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CBasicTerrain::Ready_FixedComponents(void* pArg)
{	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;
	
	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Terrain */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Terrain, TEXT("Prototype_Component_Terrain"))))
		return E_FAIL;

	if (nullptr == pArg)
	{
		if (FAILED(static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeJustGrid(1024U, 1024U, 64U, 64U)))
			return E_FAIL;

		if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxDebug"))))
			return E_FAIL;
		GetShader()->SetPassIndex(3);
	}
	else
	{
		if (FAILED(static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeWithHeightMap(*reinterpret_cast<wstring*>(pArg))))
			return E_FAIL;

		if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxNorTex"))))
			return E_FAIL;
		GetShader()->SetPassIndex(0);

		if (FAILED(GetShader()->Bind_RawValue("g_vMtrlDiffuse", &Colors::MediumSeaGreen, sizeof(Color))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBasicTerrain::Ready_Scripts(void* pArg)
{
	return S_OK;
}

HRESULT CBasicTerrain::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ))/* ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 0))*/)
	{
		return E_FAIL;
	}

	return S_OK;
}

CBasicTerrain* CBasicTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBasicTerrain* pInstance = new CBasicTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBasicTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBasicTerrain* CBasicTerrain::Clone(void* pArg)
{
	CBasicTerrain* pInstance = new CBasicTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBasicTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBasicTerrain::Free()
{
	Super::Free();
}
