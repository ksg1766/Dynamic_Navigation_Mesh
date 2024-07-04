#include "stdafx.h"
#include "..\Public\Moloch.h"
#include "GameInstance.h"
#include "MonsterController.h"
#include "MonsterStats.h"
#include "Moloch_Sword.h"

#include "BT_Composite.h"
#include "Moloch_BT_IF_Dead.h"
#include "Moloch_BT_REPEAT.h"
#include "Moloch_BT_WHILE_Phase1.h"
#include "Moloch_BT_WHILE_Phase2.h"

#include "Moloch_BT_Dash.h"
#include "Moloch_BT_Swipe.h"
#include "Moloch_BT_Dead.h"
#include "Moloch_BT_Idle.h"
#include "Moloch_BT_Chase.h"

#include "Moloch_BT_IF_StartP2.h"
#include "Moloch_BT_Eruption1.h"

#include "Moloch_BT_TremorPulse.h"
#include "Moloch_BT_FullDash1.h"
#include "Moloch_BT_Swing1.h"
#include "Moloch_BT_Swing2.h"

#include "Moloch_BT_IF_StartP3.h"
#include "Moloch_BT_Swing3.h"

#include "Moloch_BT_FullDash2.h"
#include "Moloch_BT_Geyser1.h"
#include "Moloch_BT_Geyser2.h"

CMoloch::CMoloch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CMoloch::CMoloch(const CMoloch& rhs)
	: Super(rhs)
{
}

HRESULT CMoloch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoloch::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	GetRigidBody()->GetSphereCollider()->SetRadius(3.6f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(1.8f, 2.5f, 1.8f));

	return S_OK;
}

void CMoloch::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CMoloch::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{
		AddRenderGroup();
	}
}

void CMoloch::DebugRender()
{
	Super::DebugRender();
}

HRESULT CMoloch::Render()
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

HRESULT CMoloch::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	_int iPass = GetShader()->GetPassIndex();
	//if (!IsDead())
		GetShader()->SetPassIndex(5);

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

	GetShader()->SetPassIndex(iPass);

	return S_OK;
}

HRESULT CMoloch::AddRenderGroup()
{
	GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CMoloch::Ready_Parts()
{
	CGameObject* pGameObject = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Moloch_Sword"), LAYERTAG::IGNORECOLLISION);
	if (nullptr == pGameObject)	return E_FAIL;
	m_vecParts.push_back(pGameObject);
	GetModel()->EquipParts(0, pGameObject->GetModel());

	return S_OK;
}

HRESULT CMoloch::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxTexFetchAnim"))))
		return E_FAIL;

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
		pNaviDesc.iCurrentIndex = 654;

		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::NavMeshAgent, TEXT("Prototype_Component_NavMeshAgent"), &pNaviDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMoloch::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_BossController */
		//if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_BossController"))))
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterController"))))
			return E_FAIL;

		m_pController = dynamic_cast<CMonsterController*>(m_vecScripts[0]);

		/* Com_MonsterStats */
		CMonsterStats::MONSTERSTAT stats = { 3000, 20 };
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_MonsterStats"), &stats)))
			return E_FAIL;

		m_pController->SetStats(dynamic_cast<CMonsterStats*>(m_vecScripts[1]));

		///* Com_BehaviorTree */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_BehaviorTree"))))
			return E_FAIL;

		CBehaviorTree* pBehaviorTree = dynamic_cast<CBehaviorTree*>(m_vecScripts[2]);
		{
			CBT_Action::BEHAVEANIMS desc;

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Impact_Stun"));
			CBT_Action* pDead = CMoloch_BT_Dead::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pIfDead = CMoloch_BT_IF_Dead::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);//죽었는가
			pIfDead->AddChild(pDead);

			// Phase1
			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Dash_Strike"));
			CBT_Action* pDash = CMoloch_BT_Dash::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Run_F"));
			CBT_Action* pChase = CMoloch_BT_Chase::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Swipe_01"));
			CBT_Action* pSwipe = CMoloch_BT_Swipe::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pChaseAttack = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			pChaseAttack->AddChild(pChase);
			pChaseAttack->AddChild(pSwipe);

			CBT_Decorator* pRepeatChaseAttack = CMoloch_BT_REPEAT::Create(this, pBehaviorTree, m_pController, 2);
			pRepeatChaseAttack->AddChild(pChaseAttack);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Idle"));
			CBT_Action* pIdle = CMoloch_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pPhase1 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);	//
			pPhase1->AddChild(pDash);
			pPhase1->AddChild(pRepeatChaseAttack);
			pPhase1->AddChild(pIdle);

			CBT_Decorator* pWhilePhase1 = CMoloch_BT_WHILE_Phase1::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::WHILE); // Phase1
			pWhilePhase1->AddChild(pPhase1);

			// Phase2
			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_CrystalEruption"));
			CBT_Action* pEruption1 = CMoloch_BT_Eruption1::Create(this, pBehaviorTree, desc, m_pController);
			
			/*desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_CrystalEruption_02"));
			CBT_Action* pEruption2 = CMoloch_BT_Eruption2::Create(this, pBehaviorTree, desc, m_pController);*/
			
			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_TremorPulse"));
			CBT_Action* pPulse = CMoloch_BT_TremorPulse::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pEnterPhase2 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			pEnterPhase2->AddChild(pEruption1);
			//pEnterPhase2->AddChild(pEruption2);
			pEnterPhase2->AddChild(pPulse);

			CBT_Decorator* pIfEnterPhase = CMoloch_BT_IF_StartP2::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);	// 2페 시작패턴 조건
			pIfEnterPhase->AddChild(pEnterPhase2);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Dash_Strike"));
			CBT_Action* pFullDash = CMoloch_BT_FullDash1::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Run_F"));
			CBT_Action* pChase2 = CMoloch_BT_Chase::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Swing_01"));
			CBT_Action* pSwing1 = CMoloch_BT_Swing1::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Swing_02"));
			CBT_Action* pSwing2 = CMoloch_BT_Swing2::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pChaseSwing = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			pChaseSwing->AddChild(pChase2);
			pChaseSwing->AddChild(pSwing1);
			pChaseSwing->AddChild(pSwing2);

			CBT_Decorator* pRepeatChaseSwing = CMoloch_BT_REPEAT::Create(this, pBehaviorTree, m_pController, 2);
			pRepeatChaseSwing->AddChild(pChaseSwing);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Idle"));
			CBT_Action* pIdle2 = CMoloch_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pPhase2 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);	//
			pPhase2->AddChild(pFullDash);
			pPhase2->AddChild(pRepeatChaseSwing);
			pPhase2->AddChild(pIdle2);

			CBT_Composite* pStartOrRun = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);	//
			pStartOrRun->AddChild(pIfEnterPhase);
			pStartOrRun->AddChild(pPhase2);

			CBT_Decorator* pWhilePhase2 = CMoloch_BT_WHILE_Phase2::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::WHILE); // Phase1
			pWhilePhase2->AddChild(pStartOrRun);

			// Phase3
			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Geyser"));
			CBT_Action* pGeyser1 = CMoloch_BT_Geyser1::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Decorator* pRepeatGeyser = CMoloch_BT_REPEAT::Create(this, pBehaviorTree, m_pController, 2);
			pRepeatGeyser->AddChild(pGeyser1);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Geyser_02"));
			CBT_Action* pGeyser2 = CMoloch_BT_Geyser2::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pEnterPhase3 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			pEnterPhase3->AddChild(pRepeatGeyser);
			pEnterPhase3->AddChild(pGeyser2);

			CBT_Decorator* pIfEnterPhase3 = CMoloch_BT_IF_StartP3::Create(this, pBehaviorTree, m_pController, CBT_Decorator::DecoratorType::IF);	// 3페 시작패턴 조건
			pIfEnterPhase3->AddChild(pEnterPhase3);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Dash_Strike_02"));
			CBT_Action* pFullDash2 = CMoloch_BT_FullDash2::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Run_F"));
			CBT_Action* pChase3 = CMoloch_BT_Chase::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Swing_01"));
			CBT_Action* pSwing1_ = CMoloch_BT_Swing1::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Swing_02"));
			CBT_Action* pSwing2_ = CMoloch_BT_Swing2::Create(this, pBehaviorTree, desc, m_pController);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Atk_Full_Swing_03"));
			CBT_Action* pSwing3_ = CMoloch_BT_Swing3::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pChaseFullSwing = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);
			pChaseFullSwing->AddChild(pChase3);
			pChaseFullSwing->AddChild(pSwing1_);
			pChaseFullSwing->AddChild(pSwing2_);
			pChaseFullSwing->AddChild(pSwing3_);

			desc.vecAnimations.clear();
			desc.vecAnimations.push_back(TEXT("Moloch_Full_Idle"));
			CBT_Action* pIdle3 = CMoloch_BT_Idle::Create(this, pBehaviorTree, desc, m_pController);

			CBT_Composite* pPhase3 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SEQUENCE);	//
			pPhase3->AddChild(pFullDash2);
			pPhase3->AddChild(pChaseFullSwing);
			pPhase3->AddChild(pIdle3);

			CBT_Composite* pStartOrRun3 = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);	//
			pStartOrRun3->AddChild(pIfEnterPhase3);
			pStartOrRun3->AddChild(pPhase3);

			//////////
			CBT_Composite* pRoot = CBT_Composite::Create(this, pBehaviorTree, m_pController, CBT_Composite::CompositeType::SELECTOR);
			pRoot->AddChild(pIfDead);
			pRoot->AddChild(pWhilePhase1);
			pRoot->AddChild(pWhilePhase2);
			pRoot->AddChild(pStartOrRun3);
			pBehaviorTree->SetRoot(pRoot);

			// BlackBoard
			BLACKBOARD& hashBlackBoard = pBehaviorTree->GetBlackBoard();
			hashBlackBoard.emplace(TEXT("AttackRange"), new tagBlackBoardData<_float>(5.f));
			hashBlackBoard.emplace(TEXT("Sight"), new tagBlackBoardData<_float>(30.f));
		}
	}

	return S_OK;
}

void CMoloch::OnCollisionEnter(CGameObject* pOther)
{
	m_pController->OnCollisionEnter(pOther);
}

void CMoloch::OnCollisionStay(CGameObject* pOther)
{
	m_pController->OnCollisionStay(pOther);
}

void CMoloch::OnCollisionExit(CGameObject* pOther)
{
	m_pController->OnCollisionExit(pOther);
}

HRESULT CMoloch::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
		return E_FAIL;

	//if (!IsDead())
		//GetShader()->SetPassIndex(0);

	return S_OK;
}

CMoloch* CMoloch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoloch* pInstance = new CMoloch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMoloch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoloch::Clone(void* pArg)
{
	CMoloch* pInstance = new CMoloch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMoloch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch::Free()
{
	Super::Free();
	for (auto& iter : m_vecParts)
		Safe_Release(iter);

	m_vecParts.clear();
}
