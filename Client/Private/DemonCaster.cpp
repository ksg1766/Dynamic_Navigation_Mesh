#include "stdafx.h"
#include "..\Public\DemonCaster.h"
#include "GameInstance.h"
#include "MonsterController.h"
#include "MonsterStats.h"

#include "BT_Composite.h"
//#include "BT_Decorator.h"
#include "DemonCaster_BT_IF_Dead.h"
#include "DemonCaster_BT_IF_InSight.h"

#include "DemonCaster_BT_Spawn.h"
#include "DemonCaster_BT_Dead.h"
#include "DemonCaster_BT_StepBack.h"
#include "DemonCaster_BT_Spell.h"
#include "DemonCaster_BT_Idle.h"
#include "DemonCaster_BT_Roaming.h"

CDemonCaster::CDemonCaster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CDemonCaster::CDemonCaster(const CDemonCaster& rhs)
	: Super(rhs)
{
}

HRESULT CDemonCaster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDemonCaster::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetRigidBody()->GetSphereCollider()->SetRadius(1.f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(1.f, 1.f, 0.8f));

	return S_OK;
}

void CDemonCaster::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CDemonCaster::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);
}

void CDemonCaster::DebugRender()
{
	Super::DebugRender();
}

HRESULT CDemonCaster::Render()
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

HRESULT CDemonCaster::RenderInstance()
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

HRESULT CDemonCaster::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	GetShader()->SetPassIndex(3);

	return S_OK;
}

HRESULT CDemonCaster::AddRenderGroup()
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

HRESULT CDemonCaster::Ready_FixedComponents()
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

HRESULT CDemonCaster::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_MonsterController */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterController"))))
			return E_FAIL;

		m_pController = dynamic_cast<CMonsterController*>(m_vecScripts[0]);

		/* Com_MonsterStats */
		CMonsterStats::MONSTERSTAT stats = { 70, 5 };
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
			desc.vecAnimations.push_back(TEXT("DemonCaster_Death"));
			CBT_Action* pDead = CDemonCaster_BT_Dead::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pIfDead = CDemonCaster_BT_IF_Dead::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//죽었는가
			pIfDead->AddChild(pDead);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("DemonCaster_Atk_Spell_02"));
			CBT_Action* pSpell = CDemonCaster_BT_Spell::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pIfInSight = CDemonCaster_BT_IF_InSight::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//If 시야내 or Hit (==Awake)
			if (FAILED(pIfInSight->AddChild(pSpell))) return E_FAIL;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("DemonCaster_Idle"));
			CBT_Action* pIdle = CDemonCaster_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("DemonCaster_Fly_F"));
			CBT_Action* pRoaming = CDemonCaster_BT_Roaming::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pPeace = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			if (FAILED(pPeace->AddChild(pIdle))) return E_FAIL;
			if (FAILED(pPeace->AddChild(pRoaming))) return E_FAIL;

			// Spawn 보류
			//desc.vecAnimations.push_back(TEXT("DemonCaster_Spawn"));

			CBT_Composite* pRoot = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);
			if (FAILED(pRoot->AddChild(pIfDead))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pIfInSight))) return E_FAIL;
			if (FAILED(pRoot->AddChild(pPeace))) return E_FAIL;
			pBehaviorTree->SetRoot(pRoot);

			BLACKBOARD& hashBlackBoard = pBehaviorTree->GetBlackBoard();
			hashBlackBoard.emplace(TEXT("Sight"), new tagBlackBoardData<_float>(14.f));
		}
	}

	return S_OK;
}

void CDemonCaster::OnCollisionEnter(CGameObject* pOther)
{
	m_pController->OnCollisionEnter(pOther);
}

void CDemonCaster::OnCollisionStay(CGameObject* pOther)
{
	m_pController->OnCollisionStay(pOther);
}

void CDemonCaster::OnCollisionExit(CGameObject* pOther)
{
	m_pController->OnCollisionExit(pOther);
}

HRESULT CDemonCaster::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CDemonCaster* CDemonCaster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDemonCaster* pInstance = new CDemonCaster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDemonCaster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDemonCaster::Clone(void* pArg)
{
	CDemonCaster* pInstance = new CDemonCaster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDemonCaster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDemonCaster::Free()
{
	Super::Free();
}
