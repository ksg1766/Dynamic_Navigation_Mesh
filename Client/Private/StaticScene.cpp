#include "stdafx.h"
#include "..\Public\StaticScene.h"
#include "GameInstance.h"

CStaticScene::CStaticScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStaticScene::CStaticScene(const CStaticScene& rhs)
	: Super(rhs)
{
}

HRESULT CStaticScene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticScene::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CStaticScene::Tick(_float fTimeDelta)
{
	/*if (m_bRendered)
		return;*/

	Super::Tick(fTimeDelta);
}

void CStaticScene::LateTick(_float fTimeDelta)
{
	if (m_bRendered)
		return;

	CGameObject::LateTick(fTimeDelta);

	//GetRenderer()->Add_RenderGroup(CRenderer::RG_SHADOW, this);
	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_bRendered = true;
}

void CStaticScene::DebugRender()
{
}

HRESULT CStaticScene::Render()
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

HRESULT CStaticScene::RenderInstance()
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

HRESULT CStaticScene::Ready_FixedComponents()
{
	///* Com_Shader */
	//if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
	//	return E_FAIL;
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMeshInstancing"))))
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


	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{/* Com_RigidBody */
		if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidStatic"))))
			return E_FAIL;
		if (FAILED(GetRigidBody()->InitializeCollider()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticScene::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStaticScene::Bind_ShaderResources()
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

CStaticScene* CStaticScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticScene* pInstance = new CStaticScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStaticScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaticScene::Clone(void* pArg)
{
	CStaticScene* pInstance = new CStaticScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStaticScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticScene::Free()
{
	Super::Free();
}
