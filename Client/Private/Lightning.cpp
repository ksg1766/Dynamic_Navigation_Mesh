#include "stdafx.h"
#include "..\Public\Lightning.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ParticleController.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CLightning::CLightning(const CLightning& rhs)
	: Super(rhs)
{
}

HRESULT CLightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightning::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;
	
	GetTransform()->Translate(Vec3(0.f, 7.f, 0.f));
	//GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	//GetRigidBody()->GetSphereCollider()->SetRadius(3.5f);
	//GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(2.f, 2.f, 2.f));

	return S_OK;
}

void CLightning::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 1.2f)
	{
		m_pGameInstance->DeleteObject(this);
	}

	if (m_fFrameTime > 0.1f && m_fFrameTime < 0.32f)
	{
		CParticleController::PARTICLE_DESC tParticleDesc;
		tParticleDesc.eType = CParticleController::ParticleType::FLOAT;
		tParticleDesc.vSpeedMax = _float3(3.7f, 3.05f, 3.7f);
		tParticleDesc.vSpeedMin = _float3(-3.7f, -1.2f, -3.7f);
		tParticleDesc.fLifeTimeMin = 1.55f;
		tParticleDesc.fLifeTimeMax = 2.55f;
		tParticleDesc.fScaleMin = 0.125f;
		tParticleDesc.fScaleMax = 0.25f;
		tParticleDesc.iPass = 1;
		tParticleDesc.vRange = Vec3(2.f, 16.f, 2.f);
		tParticleDesc.vCenter = GetTransform()->GetPosition();
		//for (_int i = 0; i < 2; ++i)
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
	}

	//if ((_int)(m_fFrameTime / 0.1f) % 4)
}

void CLightning::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CLightning::DebugRender()
{
	Super::DebugRender();
}

HRESULT CLightning::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_fFrameTime < 0.6f)
	{
		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.03f) % 4)))
			return E_FAIL;

		GetShader()->SetPassIndex(0);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;

		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_LightningTexture", ((_int)(m_fFrameTime / 0.03f) + 1) % 4)))
			return E_FAIL;

		GetShader()->SetPassIndex(0);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;

		if (FAILED(m_pTextureEx1->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.03f) % 4)))
			return E_FAIL;
		GetShader()->SetPassIndex(1);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureEx2->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.15f) % 4)))
			return E_FAIL;
		GetShader()->SetPassIndex(1);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;
	}

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CLightning::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Lightning"))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Point"))))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;
	
	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_Lightning_Bolts"))))
		return E_FAIL;

	m_pTextureEx1 = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning_Spearhead"), nullptr));
	m_pTextureEx2 = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning_Spark"), nullptr));
	if (!m_pTextureEx1 || !m_pTextureEx2)
		return E_FAIL;

	return S_OK;
}

HRESULT CLightning::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CLightning::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightning::Clone(void* pArg)
{
	CLightning* pInstance = new CLightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightning::Free()
{
	Safe_Release(m_pTextureEx1);
	Safe_Release(m_pTextureEx2);
	Super::Free();
}
