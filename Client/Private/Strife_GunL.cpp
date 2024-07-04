#include "stdafx.h"
#include "..\Public\Strife_GunL.h"
#include "GameInstance.h"

CStrife_GunL::CStrife_GunL(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_GunL::CStrife_GunL(const CStrife_GunL& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_GunL::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_GunL::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CStrife_GunL::Tick(const _float& fTimeDelta)
{
	
	Super::Tick(fTimeDelta);
}

void CStrife_GunL::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CStrife_GunL::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_GunL::Render()
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

HRESULT CStrife_GunL::Ready_FixedComponents()
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

HRESULT CStrife_GunL::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_GunL::Bind_ShaderResources()
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

CStrife_GunL* CStrife_GunL::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_GunL* pInstance = new CStrife_GunL(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_GunL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_GunL::Clone(void* pArg)
{
	CStrife_GunL* pInstance = new CStrife_GunL(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_GunL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_GunL::Free()
{
	Super::Free();
}
