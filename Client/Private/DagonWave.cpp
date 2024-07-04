#include "stdafx.h"
#include "..\Public\DagonWave.h"
#include "GameInstance.h"
#include "Particle_WaveSplash.h"

CDagonWave::CDagonWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CDagonWave::CDagonWave(const CDagonWave& rhs)
	: Super(rhs)
{
}

HRESULT CDagonWave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDagonWave::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	m_pSplash = static_cast<CParticle_WaveSplash*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle_WaveSplash"), LAYERTAG::IGNORECOLLISION));
	if (nullptr == m_pSplash)
		return E_FAIL;

	return S_OK;
}

void CDagonWave::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;

	if (m_fFrameTime > 10.f)
	{
		m_pGameInstance->DeleteObject(this);
		m_pGameInstance->DeleteObject(m_pSplash);
	}

	GetTransform()->Translate(4.f * fTimeDelta * GetTransform()->GetForward());
	m_pSplash->GetTransform()->SetPosition(GetTransform()->GetPosition());
	m_pSplash->SetEmitDirection(GetTransform()->GetForward());
}

void CDagonWave::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CDagonWave::DebugRender()
{
	Super::DebugRender();
}

HRESULT CDagonWave::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CDagonWave::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxTexFetchAnim"))))
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

	///* Com_RigidBody */
	//if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
	//	|| FAILED(GetRigidBody()->InitializeCollider()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDagonWave::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

void CDagonWave::OnCollisionEnter(CGameObject* pOther)
{
}

void CDagonWave::OnCollisionStay(CGameObject* pOther)
{
}

void CDagonWave::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CDagonWave::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CDagonWave* CDagonWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDagonWave* pInstance = new CDagonWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDagonWave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDagonWave::Clone(void* pArg)
{
	CDagonWave* pInstance = new CDagonWave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDagonWave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagonWave::Free()
{
	Super::Free();
}