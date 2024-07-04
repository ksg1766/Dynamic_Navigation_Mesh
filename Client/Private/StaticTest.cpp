#include "stdafx.h"
#include "..\Public\StaticTest.h"
#include "GameInstance.h"

CStaticTest::CStaticTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStaticTest::CStaticTest(const CStaticTest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CStaticTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticTest::Initialize(void * pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(40.f, 40.f, 40.f));
	GetTransform()->Translate(Vec3(0.f, 0.f, 0.f));

	return S_OK;
}

void CStaticTest::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

}

void CStaticTest::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CStaticTest::Render()
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

HRESULT CStaticTest::Ready_FixedComponents()
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
	GetShader()->SetPassIndex(2);

	/* Com_VIBuffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Cube"))))
		return E_FAIL;

	/* Com_Texture */
	/*if (FAILED(Super::AddComponent(m_pGameInstance->GetCurrentLevelIndex(), ComponentType::Texture, TEXT("Prototype_Component_Texture_Sky"))))
	{
		return E_FAIL;
	}*/

	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidStatic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticTest::Ready_Scripts()
{
	

	return S_OK;
}

HRESULT CStaticTest::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4)))/* ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 2))*/)
	{
		return E_FAIL;
	}

	return S_OK;
}

CStaticTest* CStaticTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticTest*		pInstance = new CStaticTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CStaticTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticTest::Clone(void * pArg)
{
	CStaticTest*		pInstance = new CStaticTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CStaticTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticTest::Free()
{
	Super::Free();
}
