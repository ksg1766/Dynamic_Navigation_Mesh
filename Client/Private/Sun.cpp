#include "stdafx.h"
#include "..\Public\Sun.h"
#include "GameInstance.h"

CSun::CSun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSun::CSun(const CSun& rhs)
	: Super(rhs)
{
}

HRESULT CSun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSun::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CSun::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CSun::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CSun::DebugRender()
{
}

HRESULT CSun::Render()
{
	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	GetTransform()->SetPosition(m_pGameInstance->Get_CamPosition_Vector() - XMLoadFloat4(&pLightDesc->vLightDir));

	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CSun::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sun"))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_Sun"))))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSun::Ready_Scripts()
{
	return S_OK;
}

HRESULT CSun::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSun* CSun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSun* pInstance = new CSun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSun::Clone(void* pArg)
{
	CSun* pInstance = new CSun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSun::Free()
{
	Super::Free();
}
