#include "stdafx.h"
#include "..\Public\WaterLightning.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ParticleController.h"
#include "Particle_Waterfall.h"
#include "PlayerController.h"

CWaterLightning::CWaterLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CWaterLightning::CWaterLightning(const CWaterLightning& rhs)
	: Super(rhs)
{
}

HRESULT CWaterLightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterLightning::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(1.3f, 1.5f, 1.3f));
	GetTransform()->Translate(Vec3(0.f, 5.f, 0.f));
	//GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	GetRigidBody()->GetSphereCollider()->SetRadius(3.5f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(2.f, 2.f, 2.f));

	return S_OK;
}

void CWaterLightning::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 4.3f)
	{
		m_pGameInstance->DeleteObject(this);
	}

	if (!m_LightningDead && m_fFrameTime > 0.7f)
	{
		CParticleController::PARTICLE_DESC tParticleDesc;
		tParticleDesc.eType = CParticleController::ParticleType::FLOAT;
		tParticleDesc.vSpeedMax = _float3(1.f, 0.35f, 1.f);
		tParticleDesc.vSpeedMin = _float3(-1.f, 0.25f, -1.f);
		tParticleDesc.fLifeTimeMin = 1.5f;
		tParticleDesc.fLifeTimeMax = 2.5f;
		tParticleDesc.fScaleMin = 0.19f;
		tParticleDesc.fScaleMax = 0.29f;
		tParticleDesc.iPass = 2;
		tParticleDesc.vRange = Vec3(1.7f, 13.7f, 1.7f);
		tParticleDesc.vCenter = GetTransform()->GetPosition();
		for (_int i = 0; i < 35; ++i)
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

		m_LightningDead = true;
	}

	if (m_fFrameTime > 0.1f && m_fFrameTime < 0.3f)
	{
		CParticleController::PARTICLE_DESC tParticleDesc;
		tParticleDesc.eType = CParticleController::ParticleType::FLY;
		tParticleDesc.vSpeedMax = _float3(2.f, 5.3f, 3.5f);
		tParticleDesc.vSpeedMin = _float3(-5.f, 4.3f, -3.5f);
		tParticleDesc.fLifeTimeMin = 1.8f;
		tParticleDesc.fLifeTimeMax = 2.7f;
		tParticleDesc.fScaleMin = 0.19f;
		tParticleDesc.fScaleMax = 0.29f;
		tParticleDesc.iPass = 2;
		//tParticleDesc.vRange = Vec3(2.f, 1.f, 2.f);
		tParticleDesc.vCenter = GetTransform()->GetPosition();
		//for (_int i = 0; i < 2; ++i)
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

		if (!m_bWaterSplash)
		{
			CParticle_Waterfall* pWaterfall = static_cast<CParticle_Waterfall*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle_Waterfall"), LAYERTAG::IGNORECOLLISION));
			pWaterfall->SetEmitDirection(Vec3::UnitY);
			pWaterfall->SetLifeTime(1.f);
			pWaterfall->GetTransform()->Translate(2.f * Vec3::UnitY + GetTransform()->GetPosition());
			m_bWaterSplash = true;
		}
	}
}

void CWaterLightning::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CWaterLightning::DebugRender()
{
	Super::DebugRender();
}

HRESULT CWaterLightning::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_fFrameTime < 0.7f)
	{
		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.05f) % 4)))
			return E_FAIL;

		GetShader()->SetPassIndex(2);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;

		if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_LightningTexture", ((_int)(m_fFrameTime / 0.05f) + 1) % 4)))
			return E_FAIL;

		GetShader()->SetPassIndex(2);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;

		if (FAILED(m_pTextureEx1->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.05f) % 4)))
			return E_FAIL;
		GetShader()->SetPassIndex(3);
		if (FAILED(GetShader()->Begin()))
			return E_FAIL;
		if (FAILED(GetBuffer()->Render()))
			return E_FAIL;
	}
	
	if ((_int)(m_fFrameTime / 0.3f) % 3 > 3)
	{
		if (FAILED(m_pTextureEx2->Bind_ShaderResource(GetShader(), "g_LightningTexture", (_int)(m_fFrameTime / 0.15f) % 4)))
			return E_FAIL;
		GetShader()->SetPassIndex(3);
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

HRESULT CWaterLightning::Ready_FixedComponents()
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

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	m_pTextureEx1 = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning_Spearhead")));
	m_pTextureEx2 = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning_Spark")));
	if (!m_pTextureEx1 || !m_pTextureEx2)
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterLightning::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

void CWaterLightning::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::PLAYER == eLayerTag)
	{
		CPlayerController* pPlayerController = static_cast<CPlayerController*>(pOther->GetScripts()[0]);
		pPlayerController->GetHitMessage();
	}
}

void CWaterLightning::OnCollisionStay(CGameObject* pOther)
{
}

void CWaterLightning::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CWaterLightning::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	_float	fEmissivePower = (4 + rand() % 7)/6.f;

	if (FAILED(GetShader()->Bind_RawValue("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CWaterLightning* CWaterLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterLightning* pInstance = new CWaterLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaterLightning::Clone(void* pArg)
{
	CWaterLightning* pInstance = new CWaterLightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterLightning::Free()
{
	Safe_Release(m_pTextureEx1);
	Safe_Release(m_pTextureEx2);
	Super::Free();
}
