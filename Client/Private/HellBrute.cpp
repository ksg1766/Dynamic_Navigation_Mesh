#include "stdafx.h"
#include "..\Public\HellBrute.h"
#include "GameInstance.h"
#include "MonsterController.h"
#include "MonsterStats.h"

#include "BT_Composite.h"
//#include "BT_Decorator.h"
#include "HellBrute_BT_Dead.h"
#include "HellBrute_BT_Fire.h"
#include "HellBrute_BT_Melee.h"
#include "HellBrute_BT_Wait.h"
#include "HellBrute_BT_Idle.h"
#include "HellBrute_BT_Roaming.h"
#include "HellBrute_BT_IF_Dead.h"
#include "HellBrute_BT_IF_InSight.h"

CHellBrute::CHellBrute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CHellBrute::CHellBrute(const CHellBrute& rhs)
	: Super(rhs)
{
}

HRESULT CHellBrute::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBrute::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetRigidBody()->GetSphereCollider()->SetRadius(3.156f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(2.f, 2.f, 1.6f));

	return S_OK;
}

void CHellBrute::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CHellBrute::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);
}

void CHellBrute::DebugRender()
{
	Super::DebugRender();
}

HRESULT CHellBrute::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CHellBrute::RenderInstance()
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

HRESULT CHellBrute::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	GetShader()->SetPassIndex(3);

	return S_OK;
}

HRESULT CHellBrute::AddRenderGroup()
{
	if(IsInstance())
	{
		//GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW_INSTANCE, this);
		GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
	}
	else
		GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CHellBrute::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxAnimInstancing"))))
		return E_FAIL;
	SetInstance(true);

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

		random_device		RandomDevice;

		mt19937_64							RandomNumber(RandomDevice());
		uniform_int_distribution<_int>		iRandom(25, 75);

		pNaviDesc.iCurrentIndex = iRandom(RandomNumber);

		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::NavMeshAgent, TEXT("Prototype_Component_NavMeshAgent"), &pNaviDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHellBrute::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_MonsterController */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterController"))))
			return E_FAIL;

		m_pController = dynamic_cast<CMonsterController*>(m_vecScripts[0]);

		/* Com_MonsterStats */
		CMonsterStats::MONSTERSTAT stats = { 150, 5 };
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterStats"), &stats)))
			return E_FAIL;

		m_pController->SetStats(dynamic_cast<CMonsterStats*>(m_vecScripts[1]));

		/* Com_BehaviorTree */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_BehaviorTree"))))
			return E_FAIL;

		CBehaviorTree* pBehaviorTree = dynamic_cast<CBehaviorTree*>(m_vecScripts[2]);
		{
			CBT_Action::BEHAVEANIMS desc;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("HellBrute_Death"));
			CBT_Action* pDead = CHellBrute_BT_Dead::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pIfDead = CHellBrute_BT_IF_Dead::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//죽었는가
			pIfDead->AddChild(pDead);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("HellBrute_Atk_Melee_01"));
			CBT_Action* pMelee = CHellBrute_BT_Melee::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("HellBrute_Taunt"));
			CBT_Action* pWait = CHellBrute_BT_Wait::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pInAtkRng = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);//사거리 내에 있을 경우
			if (FAILED(pInAtkRng->AddChild(pWait))) return E_FAIL;
			if (FAILED(pInAtkRng->AddChild(pMelee))) return E_FAIL;

			desc.vecAnimations.clear();
			//desc.vecAnimations.push_back(TEXT("HellBrute_Atk_Gattling_Start"));
			desc.vecAnimations.push_back(TEXT("HellBrute_Atk_Gattling_Fire"));
			//desc.vecAnimations.push_back(TEXT("HellBrute_Atk_Gattling_End"));
			CBT_Action* pFire = CHellBrute_BT_Fire::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pInSightTrue = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);//시야 내에 있을 경우
			if (FAILED(pInSightTrue->AddChild(pFire))) return E_FAIL;
			if (FAILED(pInSightTrue->AddChild(pInAtkRng))) return E_FAIL;

			CBT_Decorator* pIfInSight = CHellBrute_BT_IF_InSight::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//If 시야내 or Hit (==Awake)
			if (FAILED(pIfInSight->AddChild(pInSightTrue))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("HellBrute_Idle"));
			CBT_Action* pIdle = CHellBrute_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("HellBrute_Walk_F"));
			CBT_Action* pRoaming = CHellBrute_BT_Roaming::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pPeace = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE); //평화로운 상태
			if (FAILED(pPeace->AddChild(pIdle))) return E_FAIL;
			if (FAILED(pPeace->AddChild(pRoaming))) return E_FAIL;

			// Spawn 보류
			//desc.vecAnimations.push_back(TEXT("HellBrute_Spawn"));

			CBT_Composite* pRoot = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);
			if (FAILED(pRoot->AddChild(pIfDead))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pIfInSight))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pPeace))) return E_FAIL;
			pBehaviorTree->SetRoot(pRoot);

			BLACKBOARD& hashBlackBoard = pBehaviorTree->GetBlackBoard();
			hashBlackBoard.emplace(TEXT("Sight"), new tagBlackBoardData<_float>(17.f));
			hashBlackBoard.emplace(TEXT("MeleeRange"), new tagBlackBoardData<_float>(7.f));
		}
	}

	return S_OK;
}

void CHellBrute::OnCollisionEnter(CGameObject* pOther)
{
	m_pController->OnCollisionEnter(pOther);
}

void CHellBrute::OnCollisionStay(CGameObject* pOther)
{
	m_pController->OnCollisionStay(pOther);
}

void CHellBrute::OnCollisionExit(CGameObject* pOther)
{
	m_pController->OnCollisionExit(pOther);
}

HRESULT CHellBrute::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}

	_float4 vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
		if (FAILED(GetShader()->Bind_RawValue("g_vLightDir", &vLightDir, sizeof(_float4))))
			return E_FAIL;
	
	_float4	vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightDiffuse", &vDiffuse, sizeof(_float4))))
		return E_FAIL;

	_float4 vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightAmbient", &vAmbient, sizeof(_float4))))
		return E_FAIL;

	_float4 vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightSpecular", &vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CHellBrute* CHellBrute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHellBrute* pInstance = new CHellBrute(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBrute");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBrute::Clone(void* pArg)
{
	CHellBrute* pInstance = new CHellBrute(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHellBrute");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBrute::Free()
{
	Super::Free();
}
