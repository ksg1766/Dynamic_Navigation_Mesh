#include "stdafx.h"
#include "..\Public\Agent.h"
#include "GameInstance.h"

#include "AgentController.h"
#include "StateMachine.h"

CAgent::CAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CAgent::CAgent(const CAgent& rhs)
	: Super(rhs)
{
}

HRESULT CAgent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAgent::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAgent::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CAgent::LateTick(_float fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	if (FAILED(AddRenderGroup()))
		__debugbreak();
}

void CAgent::DebugRender()
{
	Super::DebugRender();
}

HRESULT CAgent::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	//DebugRender();
#endif

	return S_OK;
}

HRESULT CAgent::AddRenderGroup()
{
	if (FAILED(GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAgent::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;
	GetShader()->SetPassIndex(0);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_Sphere"))))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_NavMeshAgent */
		CNavMeshAgent::NAVIGATION_DESC pNaviDesc;
		pNaviDesc.iCurrentIndex = 0;

		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::NavMeshAgent, TEXT("Prototype_Component_NavMeshAgent"), &pNaviDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAgent::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_AgentController */
		if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_AgentController"), pArg)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAgent::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}
	
	GetShader()->SetPassIndex(4);

	return S_OK;
}

CAgent* CAgent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAgent* pInstance = new CAgent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAgent::Clone(void* pArg)
{
	CAgent* pInstance = new CAgent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAgent::Free()
{
	Super::Free();
}
