#include "stdafx.h"
#include "..\Public\StaticBase.h"
#include "GameInstance.h"

CStaticBase::CStaticBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStaticBase::CStaticBase(const CStaticBase& rhs)
	: Super(rhs)
{
}

HRESULT CStaticBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticBase::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CStaticBase::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CStaticBase::LateTick(_float fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
}

void CStaticBase::DebugRender()
{
}

HRESULT CStaticBase::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CStaticBase::RenderInstance()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CStaticBase::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMeshInstancing"))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticBase::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStaticBase::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CStaticBase* CStaticBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticBase* pInstance = new CStaticBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStaticBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaticBase::Clone(void* pArg)
{
	CStaticBase* pInstance = new CStaticBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStaticBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticBase::Free()
{
	Super::Free();
}
