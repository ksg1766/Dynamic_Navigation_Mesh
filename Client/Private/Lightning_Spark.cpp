#include "stdafx.h"
#include "..\Public\Lightning_Spark.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ParticleController.h"
#include "Particle_Waterfall.h"
#include "PlayerController.h"

CLightning_Spark::CLightning_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CLightning_Spark::CLightning_Spark(const CLightning_Spark& rhs)
	: Super(rhs)
{
}

HRESULT CLightning_Spark::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightning_Spark::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(1.75f, 3.3f, 1.75f));
	GetTransform()->RotateYAxisFixed(Vec3(0.f, rand(), 0.f));

	GetRigidBody()->GetSphereCollider()->SetRadius(3.5f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(2.f, 2.f, 2.f));

	return S_OK;
}

void CLightning_Spark::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.9f)
	{
		m_pGameInstance->DeleteObject(this);
	}

	if (0.1f < m_fFrameTime && m_fFrameTime < 0.6f)
	{
		CParticleController::PARTICLE_DESC tParticleDesc;
		tParticleDesc.eType = CParticleController::ParticleType::EXPLODE;
		tParticleDesc.vSpeedMax = _float3(3.3f, 10.7f, 3.3f);
		tParticleDesc.vSpeedMin = _float3(-3.3f, 7.7f, -3.3f);
		tParticleDesc.fLifeTimeMax = 1.7f;
		tParticleDesc.fLifeTimeMin = 1.2f;
		tParticleDesc.fScaleMin = 0.15f;
		tParticleDesc.fScaleMax = 0.27f;
		tParticleDesc.iPass = 2;
		tParticleDesc.vRange = Vec3(3.5f, 1.f, 3.5f);
		tParticleDesc.vCenter = GetTransform()->GetPosition();
		//for (_int i = 0; i < 2; ++i)
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

		tParticleDesc.vSpeedMax = _float3(3.3f, 7.7f, 3.3f);
		tParticleDesc.vSpeedMin = _float3(-3.3f, 5.7f, -3.3f);
		tParticleDesc.fScaleMin = 0.13f;
		tParticleDesc.fScaleMax = 0.23f;

		tParticleDesc.eType = CParticleController::ParticleType::FLOAT;
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

	//GetTransform()->RotateYAxisFixed(Vec3(0.f, rand(), 0.f));
}

void CLightning_Spark::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CLightning_Spark::DebugRender()
{
	Super::DebugRender();
}

HRESULT CLightning_Spark::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (0 == ((_int)(m_fFrameTime / 0.04f) % 4) % 2)
	{
		for (_int i = 0; i < 4; ++i)
		{
			if (rand() % 10 <= 7)
				m_byteFlag ^= 1 << i;
		}

		GetTransform()->RotateYAxisFixed(Vec3(0.f, rand(), 0.f));
	}

	for (_int i = 0; i < 4; ++i)
	{
		if (m_byteFlag & 1 << i)
			m_pSparkModels[i]->Render();

		GetTransform()->RotateYAxisFixed(Vec3(0.f, 90.f, 0.f));
	}

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CLightning_Spark::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(3);

	m_pSparkModels[0] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark0")));
	m_pSparkModels[1] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark1")));
	m_pSparkModels[2] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark2")));
	m_pSparkModels[3] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark3")));

	for (_int i = 0; i < 4; ++i)
		if(nullptr == m_pSparkModels[i])
			return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightning_Spark::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CLightning_Spark::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_fEmissivePower", &m_fEmissivePower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CLightning_Spark::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::PLAYER == eLayerTag)
	{
		CPlayerController* pPlayerController = static_cast<CPlayerController*>(pOther->GetScripts()[0]);
		pPlayerController->GetHitMessage();
	}
}

void CLightning_Spark::OnCollisionStay(CGameObject* pOther)
{
}

void CLightning_Spark::OnCollisionExit(CGameObject* pOther)
{
}

CLightning_Spark* CLightning_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning_Spark* pInstance = new CLightning_Spark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightning_Spark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightning_Spark::Clone(void* pArg)
{
	CLightning_Spark* pInstance = new CLightning_Spark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightning_Spark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightning_Spark::Free()
{
	for(_int i = 0; i < 4; ++i)
		Safe_Release(m_pSparkModels[i]);

	Super::Free();
}
