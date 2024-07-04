#include "stdafx.h"
#include "..\Public\GeyserCrack.h"
#include "GameInstance.h"

CGeyserCrack::CGeyserCrack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CGeyserCrack::CGeyserCrack(const CGeyserCrack& rhs)
	: Super(rhs)
{
}

HRESULT CGeyserCrack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGeyserCrack::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(15.f, 15.f, 1.f));
	GetTransform()->Rotate(Vec3(90.f, 0.f, 0.f));

	return S_OK;
}

void CGeyserCrack::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CGeyserCrack::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CGeyserCrack::DebugRender()
{
	Super::DebugRender();
}

HRESULT CGeyserCrack::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetShader()->Begin()))
		return E_FAIL;

	if (FAILED(GetBuffer()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CGeyserCrack::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxPosTex"))))
		return E_FAIL;
	GetShader()->SetPassIndex(4);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Rect"))))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;
	
	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_GeyserCrack"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGeyserCrack::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CGeyserCrack::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResources(GetShader(), "g_Textures")))
		return E_FAIL;

	return S_OK;
}

CGeyserCrack* CGeyserCrack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGeyserCrack* pInstance = new CGeyserCrack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGeyserCrack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGeyserCrack::Clone(void* pArg)
{
	CGeyserCrack* pInstance = new CGeyserCrack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGeyserCrack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGeyserCrack::Free()
{
	Super::Free();
}
