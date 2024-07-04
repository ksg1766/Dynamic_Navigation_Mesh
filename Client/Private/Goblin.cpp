#include "stdafx.h"
#include "..\Public\Goblin.h"
#include "GameInstance.h"
#include "MonsterController.h"
#include "MonsterStats.h"

#include "BT_Composite.h"
//#include "BT_Decorator.h"
#include "Goblin_BT_IF_Dead.h"
#include "Goblin_BT_IF_InSight.h"

#include "Goblin_BT_Spawn.h"
#include "Goblin_BT_Dead.h"
#include "Goblin_BT_Wait.h"
#include "Goblin_BT_Suicide.h"
#include "Goblin_BT_Idle.h"
#include "Goblin_BT_Chase.h"

CGoblin::CGoblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CGoblin::CGoblin(const CGoblin& rhs)
	: Super(rhs)
{
}

HRESULT CGoblin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetRigidBody()->GetSphereCollider()->SetRadius(1.f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(1.f, 1.f, 0.8f));

	return S_OK;
}

void CGoblin::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CGoblin::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);
}

void CGoblin::DebugRender()
{
	Super::DebugRender();
}

HRESULT CGoblin::Render()
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

HRESULT CGoblin::RenderInstance()
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

HRESULT CGoblin::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	GetShader()->SetPassIndex(3);

	return S_OK;
}

HRESULT CGoblin::AddRenderGroup()
{
	if (IsInstance())
	{
		//GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW_INSTANCE, this);
		GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
	}
	else
		GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CGoblin::Ready_FixedComponents()
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

HRESULT CGoblin::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_MonsterController */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterController"))))
			return E_FAIL;

		m_pController = dynamic_cast<CMonsterController*>(m_vecScripts[0]);

		/* Com_MonsterStats */
		CMonsterStats::MONSTERSTAT stats = { 60, 30 };
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
			desc.vecAnimations.push_back(TEXT("Goblin_Death_Impact_Explode"));
			CBT_Action* pDead = CGoblin_BT_Dead::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pIfDead = CGoblin_BT_IF_Dead::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//죽었는가
			pIfDead->AddChild(pDead);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Goblin_Acid_Death_Explode_Short"));
			CBT_Action* pSuicide = CGoblin_BT_Suicide::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Goblin_Atk_SuicideBomb_Run"));
			CBT_Action* pChase = CGoblin_BT_Chase::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pInSightTrue = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);//시야 내에 있을 경우
			if (FAILED(pInSightTrue->AddChild(pChase))) return E_FAIL;
			if (FAILED(pInSightTrue->AddChild(pSuicide))) return E_FAIL;

			CBT_Decorator* pIfInSight = CGoblin_BT_IF_InSight::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//If 시야내 or Hit (==Awake)
			if (FAILED(pIfInSight->AddChild(pInSightTrue))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Goblin_Idle"));
			CBT_Action* pIdle = CGoblin_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			// Spawn 보류
			//desc.vecAnimations.push_back(TEXT("Goblin_Spawn"));

			CBT_Composite* pRoot = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);
			if (FAILED(pRoot->AddChild(pIfDead))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pIfInSight))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pIdle))) return E_FAIL;
			pBehaviorTree->SetRoot(pRoot);

			BLACKBOARD& hashBlackBoard = pBehaviorTree->GetBlackBoard();
			hashBlackBoard.emplace(TEXT("Sight"), new tagBlackBoardData<_float>(10.f));
			hashBlackBoard.emplace(TEXT("SuicideRange"), new tagBlackBoardData<_float>(1.5f));
		}
	}

	return S_OK;
}

void CGoblin::OnCollisionEnter(CGameObject* pOther)
{
	m_pController->OnCollisionEnter(pOther);
}

void CGoblin::OnCollisionStay(CGameObject* pOther)
{
	m_pController->OnCollisionStay(pOther);
}

void CGoblin::OnCollisionExit(CGameObject* pOther)
{
	m_pController->OnCollisionExit(pOther);
}

HRESULT CGoblin::Bind_ShaderResources()
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

CGoblin* CGoblin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin* pInstance = new CGoblin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin::Clone(void* pArg)
{
	CGoblin* pInstance = new CGoblin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin::Free()
{
	Super::Free();
}
