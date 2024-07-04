#include "stdafx.h"
#include "..\Public\TremorCrystal.h"
#include "GameInstance.h"
#include "Wave_Ring.h"
#include "Particle.h"
#include "ParticleController.h"
#include "GeyserCrack.h"

CTremorCrystal::CTremorCrystal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CTremorCrystal::CTremorCrystal(const CTremorCrystal& rhs)
	: Super(rhs)
{
}

HRESULT CTremorCrystal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTremorCrystal::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetTransform()->SetScale(Vec3(0.7f, 0.7f, 0.7f));
	GetTransform()->RotateYAxisFixed(Vec3(0.f, 90.f, 0.f));
	GetTransform()->Translate(Vec3(0.f, 0.6f, 0.f));

	EFFECT_DESC pDesc = *reinterpret_cast<EFFECT_DESC*>(pArg);
	m_fLifeTime = pDesc.fLifeTime;

	m_pCrack = static_cast<CGeyserCrack*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_GeyserCrack"), LAYERTAG::IGNORECOLLISION));

	return S_OK;
}

void CTremorCrystal::Tick(const _float& fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	Vec3 vPos = GetTransform()->GetPosition();
	vPos.y = 7.03f;
	m_pCrack->GetTransform()->SetPosition(vPos);

	if (m_fLifeTime < 0.f)
	{
		m_pGameInstance->DeleteObject(this);
		m_pGameInstance->DeleteObject(m_pCrack);
		CParticleController::PARTICLE_DESC tParticleDesc;
		tParticleDesc.vSpeedMax = _float3(4.f, 8.5f, 4.f);
		tParticleDesc.vSpeedMin = _float3(-4.f, 5.7f, -4.f);
		tParticleDesc.fLifeTimeMin = 2.f;
		tParticleDesc.fLifeTimeMax = 2.7f;
		tParticleDesc.fScaleMax = 0.3f;
		tParticleDesc.fScaleMin = 0.15f;
		tParticleDesc.vCenter = GetTransform()->GetPosition();
		
		tParticleDesc.eType = CParticleController::ParticleType::RIGIDBODY;
		for (_int i = 0; i < 5; ++i)
		{
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_G"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_H"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_I"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_L"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_TremorCrystal_M"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
		}

		tParticleDesc.eType = CParticleController::ParticleType::EXPLODE;
		tParticleDesc.iPass = 1;
		for (_int i = 0; i < 15; ++i)
			m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

		if (m_bWithLightning)
		{
			CGameObject* pGameObject = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Lightning"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);
			pGameObject->GetTransform()->SetScale(Vec3(2.7f, 1.2f, 2.7f));
			pGameObject->GetTransform()->Translate(GetTransform()->GetPosition());
		}

		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Bubble"), LAYERTAG::UNIT_AIR)
			->GetTransform()->Translate(GetTransform()->GetPosition());
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Wave_Ring"), LAYERTAG::IGNORECOLLISION)
			->GetTransform()->Translate(GetTransform()->GetPosition());

		return;
	}

	Super::Tick(fTimeDelta);
}

void CTremorCrystal::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CTremorCrystal::DebugRender()
{
	Super::DebugRender();
}

HRESULT CTremorCrystal::Render()
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

HRESULT CTremorCrystal::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	GetShader()->SetPassIndex(5);

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

	GetShader()->SetPassIndex(0);

	return S_OK;
}

HRESULT CTremorCrystal::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_TremorCrystal_A_emissive"))))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidStatic"))))
		return E_FAIL;

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
		if(FAILED(GetRigidBody()->InitializeCollider()))
			return E_FAIL;

	return S_OK;
}

HRESULT CTremorCrystal::Ready_Scripts()
{
	return S_OK;
}

HRESULT CTremorCrystal::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_EmissiveTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CTremorCrystal* CTremorCrystal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTremorCrystal* pInstance = new CTremorCrystal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTremorCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTremorCrystal::Clone(void* pArg)
{
	CTremorCrystal* pInstance = new CTremorCrystal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTremorCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTremorCrystal::Free()
{
	Super::Free();
}
