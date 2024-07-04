#include "stdafx.h"
#include "..\Public\Strife_GunR.h"
#include "GameInstance.h"

CStrife_GunR::CStrife_GunR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_GunR::CStrife_GunR(const CStrife_GunR& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_GunR::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_GunR::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;	

	return S_OK;
}

void CStrife_GunR::Tick(const _float& fTimeDelta)
{

	Super::Tick(fTimeDelta);
}

void CStrife_GunR::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CStrife_GunR::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_GunR::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CStrife_GunR::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VTFSocket"))))
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

	/* Com_RigidBody */	// 총이라 필요없음

	return S_OK;
}

HRESULT CStrife_GunR::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_GunR::Bind_ShaderResources()
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

CStrife_GunR* CStrife_GunR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_GunR* pInstance = new CStrife_GunR(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_GunR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_GunR::Clone(void* pArg)
{
	CStrife_GunR* pInstance = new CStrife_GunR(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_GunR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_GunR::Free()
{
	Super::Free();
}
