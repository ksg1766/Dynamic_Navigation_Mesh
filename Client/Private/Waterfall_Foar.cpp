#include "stdafx.h"
#include "..\Public\Waterfall_Foar.h"
#include "GameInstance.h"

CWaterfall_Foar::CWaterfall_Foar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CWaterfall_Foar::CWaterfall_Foar(const CWaterfall_Foar& rhs)
	: Super(rhs)
{
}

HRESULT CWaterfall_Foar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterfall_Foar::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CWaterfall_Foar::Tick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::Tick(fTimeDelta);
	
	m_fFrameTime += 0.3f * fTimeDelta;
	if (m_fFrameTime > 1.f)
		m_fFrameTime -= 1.f;
}

void CWaterfall_Foar::LateTick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_bRendered = true;
}

void CWaterfall_Foar::DebugRender()
{
}

HRESULT CWaterfall_Foar::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	m_bRendered = false;

	return S_OK;
}

HRESULT CWaterfall_Foar::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
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

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_RigidBody */
		if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidStatic"))))
			return E_FAIL;
		if (FAILED(GetRigidBody()->InitializeCollider()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWaterfall_Foar::Ready_Scripts()
{
	return S_OK;
}

HRESULT CWaterfall_Foar::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) || 
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	_float2 vUVoffset(0.f, -m_fFrameTime);

	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &vUVoffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CWaterfall_Foar* CWaterfall_Foar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterfall_Foar* pInstance = new CWaterfall_Foar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterfall_Foar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaterfall_Foar::Clone(void* pArg)
{
	CWaterfall_Foar* pInstance = new CWaterfall_Foar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterfall_Foar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterfall_Foar::Free()
{
	Super::Free();
}
