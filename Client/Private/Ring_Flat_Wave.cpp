#include "stdafx.h"
#include "..\Public\Ring_Flat_Wave.h"
#include "GameInstance.h"

CRing_Flat_Wave::CRing_Flat_Wave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CRing_Flat_Wave::CRing_Flat_Wave(const CRing_Flat_Wave& rhs)
	: Super(rhs)
{
}

HRESULT CRing_Flat_Wave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRing_Flat_Wave::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CRing_Flat_Wave::Tick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::Tick(fTimeDelta);
	
	m_fFrameTime += 0.3f * fTimeDelta;
	if (m_fFrameTime > 1.f)
		m_fFrameTime -= 1.f;
}

void CRing_Flat_Wave::LateTick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
	m_bRendered = true;
}

void CRing_Flat_Wave::DebugRender()
{
}

HRESULT CRing_Flat_Wave::Render()
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

HRESULT CRing_Flat_Wave::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;
	GetShader()->SetPassIndex(7);
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

HRESULT CRing_Flat_Wave::Ready_Scripts()
{
	return S_OK;
}

HRESULT CRing_Flat_Wave::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) || 
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	_float2 vUVoffset(m_fFrameTime, 0.f);

	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &vUVoffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CRing_Flat_Wave* CRing_Flat_Wave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRing_Flat_Wave* pInstance = new CRing_Flat_Wave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRing_Flat_Wave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRing_Flat_Wave::Clone(void* pArg)
{
	CRing_Flat_Wave* pInstance = new CRing_Flat_Wave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRing_Flat_Wave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRing_Flat_Wave::Free()
{
	Super::Free();
}
