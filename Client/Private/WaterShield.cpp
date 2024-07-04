#include "stdafx.h"
#include "..\Public\WaterShield.h"
#include "GameInstance.h"

CWaterShield::CWaterShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CWaterShield::CWaterShield(const CWaterShield& rhs)
	: Super(rhs)
{
}

HRESULT CWaterShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterShield::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CWaterShield::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	
	m_fFrameTime += 0.3f * fTimeDelta;
	m_fDeleteTime += fTimeDelta;

	if (m_fDeleteTime >= 5.f)
		m_pGameInstance->DeleteObject(this);

	if (m_fFrameTime > 2.f)
		m_fFrameTime -= 1.f;

	if (m_fFrameTime <= 1.f)
	{
		if (0.f != m_fScaleRatio)
			GetTransform()->SetScale(m_fScaleRatio * Vec3(m_fFrameTime, m_fFrameTime, m_fFrameTime));
		else
			GetTransform()->SetScale(Vec3(1.3f, 1.3f, 1.3f));
	}
}

void CWaterShield::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CWaterShield::DebugRender()
{
}

HRESULT CWaterShield::Render()
{
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

HRESULT CWaterShield::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;
	GetShader()->SetPassIndex(8);
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

HRESULT CWaterShield::Ready_Scripts()
{
	return S_OK;
}

HRESULT CWaterShield::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) || 
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	_float2 vUVoffset(2.5f * m_fFrameTime, 2.5f * m_fFrameTime);

	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &vUVoffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CWaterShield* CWaterShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterShield* pInstance = new CWaterShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaterShield::Clone(void* pArg)
{
	CWaterShield* pInstance = new CWaterShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterShield::Free()
{
	Super::Free();
}
