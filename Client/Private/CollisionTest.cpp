#include "stdafx.h"
#include "..\Public\CollisionTest.h"
#include "GameInstance.h"

CCollisionTest::CCollisionTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCollisionTest::CCollisionTest(const CCollisionTest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCollisionTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollisionTest::Initialize(void * pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(40.f, 40.f, 40.f));
	GetTransform()->Translate(Vec3(0.f, 0.f, 200.f));

	return S_OK;
}

void CCollisionTest::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	//GetRigidBody()->Tick(fTimeDelta);
}

void CCollisionTest::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCollisionTest::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetShader()->Begin();

	GetBuffer()->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CCollisionTest::Ready_FixedComponents()
{
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxCubeNom"))))
		return E_FAIL;
	GetShader()->SetPassIndex(1);

	/* Com_VIBuffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Cube"))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_Sky"))))
		return E_FAIL;

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollisionTest::Ready_Scripts()
{
	/* Com_PlayerController */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Script, TEXT("Prototype_Component_TestAIController"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollisionTest::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 1)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CCollisionTest* CCollisionTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCollisionTest*		pInstance = new CCollisionTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCollisionTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCollisionTest::Clone(void * pArg)
{
	CCollisionTest*		pInstance = new CCollisionTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCollisionTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollisionTest::Free()
{
	Super::Free();
}
