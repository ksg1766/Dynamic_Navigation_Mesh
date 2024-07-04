#include "stdafx.h"
#include "..\Public\CrystalParticle.h"
#include "GameInstance.h"

CCrystalParticle::CCrystalParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CCrystalParticle::CCrystalParticle(const CCrystalParticle& rhs)
	: Super(rhs)
{
}

HRESULT CCrystalParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrystalParticle::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCrystalParticle::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CCrystalParticle::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
}

void CCrystalParticle::DebugRender()
{
}

HRESULT CCrystalParticle::Render()
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

HRESULT CCrystalParticle::RenderInstance()
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

HRESULT CCrystalParticle::Ready_FixedComponents()
{
	///* Com_Shader */
	//if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
	//	return E_FAIL;
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxNonAnimInstancing"))))
		return E_FAIL;
	GetShader()->SetPassIndex(6);
	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrystalParticle::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_ParticleController"), pArg)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CCrystalParticle::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CCrystalParticle* CCrystalParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCrystalParticle* pInstance = new CCrystalParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrystalParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrystalParticle::Clone(void* pArg)
{
	CCrystalParticle* pInstance = new CCrystalParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrystalParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrystalParticle::Free()
{
	Super::Free();
}
