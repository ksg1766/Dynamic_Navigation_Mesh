#include "stdafx.h"
#include "..\Public\P_Strife.h"
#include "GameInstance.h"

#include "PlayerController.h"
#include "StateMachine.h"
#include "P_Strife_State_Idle.h"
#include "P_Strife_State_Run.h"
#include "P_Strife_State_Aim.h"
#include "P_Strife_State_Jump.h"
#include "P_Strife_State_Dash.h"
#include "P_Strife_State_Impact.h"

//
#include "Particle_Waterfall.h"

CP_Strife::CP_Strife(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CP_Strife::CP_Strife(const CP_Strife& rhs)
	: Super(rhs)
{
}

HRESULT CP_Strife::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CP_Strife::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	
	if (FAILED(Ready_Parts()))
		return E_FAIL;

	//GetRigidBody()->GetSphereCollider()->SetRadius(10.f);
	//GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(5.f, 5.f, 5.f));
	//GetTransform()->SetScale(Vec3(2.f, 2.f, 2.f));

	//if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("mus_hell_med_drums.ogg"), CHANNELID::CHANNEL_BGM, 0.5f)))
	//	__debugbreak();

	return S_OK;
}

void CP_Strife::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::L))
	{
		CParticle_Waterfall* pWaterfallSplash = static_cast<CParticle_Waterfall*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle_Waterfall"), LAYERTAG::IGNORECOLLISION));
		pWaterfallSplash->SetEmitDirection(GetTransform()->GetUp());
		//pWaterfallSplash->SetEmitDirection(GetTransform()->GetForward());
		pWaterfallSplash->SetLifeTime(10.f);
		//pWaterLightning->GetTransform()->SetScale(Vec3(2.7f, 1.2f, 2.7f));

		//TODO :: 
		pWaterfallSplash->GetTransform()->SetPosition(3.f * Vec3::UnitY + GetTransform()->GetPosition());
		//SetEmitDirection();
		//SetLifeTime();
	}
	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::K))
	{
		CParticle_Waterfall* pWaterfallSplash = static_cast<CParticle_Waterfall*>(m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle_Waterfall"), LAYERTAG::IGNORECOLLISION));
		//pWaterfallSplash->SetEmitDirection(GetTransform()->GetUp());
		pWaterfallSplash->SetEmitDirection(GetTransform()->GetForward());
		pWaterfallSplash->SetLifeTime(10.f);
		//pWaterLightning->GetTransform()->SetScale(Vec3(2.7f, 1.2f, 2.7f));
		//TODO ::
		pWaterfallSplash->GetTransform()->SetPosition(3.f * Vec3::UnitY + GetTransform()->GetPosition());
	}
}

void CP_Strife::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);
}

void CP_Strife::DebugRender()
{
	Super::DebugRender();
}

HRESULT CP_Strife::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	//DebugRender();
#endif

	return S_OK;
}

HRESULT CP_Strife::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
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

	return S_OK;
}

HRESULT CP_Strife::AddRenderGroup()
{
	GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CP_Strife::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxTexFetchAnim"))))
		return E_FAIL;
	GetShader()->SetPassIndex(4);

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
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_NavMeshAgent */
		CNavMeshAgent::NAVIGATION_DESC pNaviDesc;
		pNaviDesc.iCurrentIndex = 0;

		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::NavMeshAgent, TEXT("Prototype_Component_NavMeshAgent"), &pNaviDesc)))
			return E_FAIL;
	}

	/* Com_Terrain */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Terrain, TEXT("Prototype_Component_Terrain"))) ||
		FAILED(static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeJustGrid(1024, 1024, 1024, 1024)))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Strife::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_PlayerController */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_PlayerController"))))
			return E_FAIL;
	
		CMonoBehaviour* pController = m_vecScripts.back();

		/* Com_StateMachine */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_StateMachine"))))
			return E_FAIL;
		
		CStateMachine* pStateMachine = dynamic_cast<CStateMachine*>(m_vecScripts.back());
		{
			CState::STATEANIMS desc;
			desc.vecAnimations.push_back(TEXT("Strife_Idle"));
			CState* pState = CP_Strife_State_Idle::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Strife_Run"));
			pState = CP_Strife_State_Run::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Strife_Aim_Idle"));
			desc.vecAnimations.push_back(TEXT("Strife_Aim_Walk"));
			desc.vecAnimations.push_back(TEXT("Strife_Aim_Walk_Back"));
			pState = CP_Strife_State_Aim::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Strife_Jump"));
			desc.vecAnimations.push_back(TEXT("Strife_Land"));
			desc.vecAnimations.push_back(TEXT("Strife_Jump_Double"));
			desc.vecAnimations.push_back(TEXT("Strife_Jump_Land_Heavy"));
			desc.vecAnimations.push_back(TEXT("Strife_Fall"));
			pState = CP_Strife_State_Jump::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Strife_Dash"));
			desc.vecAnimations.push_back(TEXT("Strife_Dash_End"));
			desc.vecAnimations.push_back(TEXT("Strife_Dash_Back"));
			desc.vecAnimations.push_back(TEXT("Strife_Dash_Back_End"));
			pState = CP_Strife_State_Dash::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Strife_Impact_FromFront"));
			desc.vecAnimations.push_back(TEXT("Strife_Impact_FromBack"));
			desc.vecAnimations.push_back(TEXT("Strife_Impact_FromLeft"));
			desc.vecAnimations.push_back(TEXT("Strife_Impact_FromRight"));
			pState = CP_Strife_State_Impact::Create(this, desc, pController);
			if (FAILED(pStateMachine->AddState(pState))) return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CP_Strife::Ready_Parts()
{
	CGameObject* pGameObject = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Strife_GunL"), LAYERTAG::IGNORECOLLISION);
	if (nullptr == pGameObject)	return E_FAIL;
	m_vecParts.push_back(pGameObject);
	if (FAILED(GetModel()->EquipParts(0, pGameObject->GetModel())))
		return E_FAIL;

	pGameObject = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Strife_GunR"), LAYERTAG::IGNORECOLLISION);
	if (nullptr == pGameObject)	return E_FAIL;
	m_vecParts.push_back(pGameObject);
	if (FAILED(GetModel()->EquipParts(1, pGameObject->GetModel())))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Strife::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}
	
	GetShader()->SetPassIndex(4);

	return S_OK;
}

void CP_Strife::OnCollisionEnter(CGameObject* pOther)
{
	m_pController->OnCollisionEnter(pOther);
}

void CP_Strife::OnCollisionStay(CGameObject* pOther)
{
	m_pController->OnCollisionStay(pOther);
}

void CP_Strife::OnCollisionExit(CGameObject* pOther)
{
	m_pController->OnCollisionExit(pOther);
}

CP_Strife* CP_Strife::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CP_Strife* pInstance = new CP_Strife(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CP_Strife");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CP_Strife::Clone(void* pArg)
{
	CP_Strife* pInstance = new CP_Strife(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CP_Strife");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Strife::Free()
{
	Super::Free();

	for (auto& iter : m_vecParts)
		Safe_Release(iter);

	m_vecParts.clear();
}
