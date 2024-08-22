#include "stdafx.h"
#include "..\Public\AIAgent.h"
#include "GameInstance.h"
#include "AIController.h"

CAIAgent::CAIAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAIAgent::CAIAgent(const CAIAgent& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAIAgent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAIAgent::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CAIAgent::Tick(_float fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CAIAgent::LateTick(_float fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	if (FAILED(AddRenderGroup()))
		__debugbreak();
}

HRESULT CAIAgent::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(GetShader()->Begin()))
		return E_FAIL;

	if (FAILED(GetBuffer()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	//DebugRender();
#endif

	return S_OK;
}

void CAIAgent::DebugRender()
{
	Super::DebugRender();
}

HRESULT CAIAgent::AddRenderGroup()
{
	if (FAILED(GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return E_FAIL;

	return S_OK;
}

void CAIAgent::AddWayPoint(const Vec3& vWayPoint)
{
	m_pController->AddWayPoint(vWayPoint);
}

HRESULT CAIAgent::Ready_FixedComponents(void* pArg)
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxCubeNom"))))
		return E_FAIL;

	/* Com_Buffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Sphere"))))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_FlatMagenta"))))
		return E_FAIL;

	/* Com_NavMeshAgent */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::NavMeshAgent, TEXT("Prototype_Component_NavMeshAgent"), pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAIAgent::Ready_Scripts()
{
	/* Com_PlayerController */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Script, TEXT("Prototype_Component_AIController"))))
		return E_FAIL;

	m_pController = static_cast<CAIController*>(m_vecScripts[0]);
	Safe_AddRef(m_pController);

	return S_OK;
}

HRESULT CAIAgent::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CAIAgent* CAIAgent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAIAgent* pInstance = new CAIAgent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAIAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAIAgent::Clone(void * pArg)
{
	CAIAgent* pInstance = new CAIAgent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAIAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAIAgent::Free()
{
	Safe_Release(m_pController);

	Super::Free();
}
