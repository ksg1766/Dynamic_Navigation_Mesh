#include "stdafx.h"
#include "..\Public\Arena.h"
#include "GameInstance.h"

CArena::CArena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CArena::CArena(const CArena& rhs)
	: Super(rhs)
{
}

HRESULT CArena::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArena::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{
		GetRigidBody()->GetSphereCollider()->SetRadius(10.f);
		GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(5.f, 5.f, 5.f));
	}

	return S_OK;
}

void CArena::Tick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::Tick(fTimeDelta);
}

void CArena::LateTick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	Super::LateTick(fTimeDelta);

	//GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_bRendered = true;
}

void CArena::DebugRender()
{
}

HRESULT CArena::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	GetModel()->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	m_bRendered = false;

	return S_OK;
}

HRESULT CArena::RenderInstance()
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

HRESULT CArena::RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)
{
	/*if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")))
		return E_FAIL;*/

	if (FAILED(GetShader()->Bind_Matrix("g_ViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_Matrix("g_ProjMatrix", &matLightProj)))
		return E_FAIL;

	GetShader()->SetPassIndex(5);

	//if (FAILED(GetModel()->Render()))
	//	return E_FAIL;

	//GetShader()->SetPassIndex(0);

	return S_OK;
}

HRESULT CArena::Ready_FixedComponents()
{
	///* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;
	/* Com_Shader */
	/*if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxNonAnimInstancing"))))
		return E_FAIL;*/

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{
		/* Com_RigidBody */
		if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidStatic"))))
			return E_FAIL;
		if (FAILED(GetRigidBody()->InitializeCollider()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CArena::Ready_Scripts()
{
	return S_OK;
}

HRESULT CArena::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CArena* CArena::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArena* pInstance = new CArena(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CArena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArena::Clone(void* pArg)
{
	CArena* pInstance = new CArena(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CArena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArena::Free()
{
	Super::Free();
}
