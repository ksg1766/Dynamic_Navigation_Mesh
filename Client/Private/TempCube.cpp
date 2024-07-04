#include "stdafx.h"
#include "..\Public\TempCube.h"
#include "GameInstance.h"

CTempCube::CTempCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTempCube::CTempCube(const CTempCube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTempCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTempCube::Initialize(void * pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(50.f, 50.f, 50.f));
	GetTransform()->Translate(Vec3(0.f, 100.f, 10.f));

	return S_OK;
}

void CTempCube::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	//GetRigidBody()->Tick(fTimeDelta);
}

void CTempCube::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTempCube::Render()
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

HRESULT CTempCube::Ready_FixedComponents()
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

	// TODO: Collider는 RigidBody의 Initialize에서 추가하고 GameObject의 컴포넌트 목록에 넣지 않아도 될것같다.
	// 업데이트는 RigidBody에서 호출해주면 된다. 시도해보자.
	/* Com_SphereCollider */
	//if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Collider, TEXT("Prototype_Component_SphereCollider"))))
	//	return E_FAIL;
	/* Com_OBBCollider */
	//if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Collider, TEXT("Prototype_Component_OBBCollider"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTempCube::Ready_Scripts()
{
	return S_OK;
}

HRESULT CTempCube::Bind_ShaderResources()
{
	//const _float4& vCampPos = pGameInstance->Get_CamPosition_Float4();

	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_DiffuseTexture", 0)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CTempCube* CTempCube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTempCube*		pInstance = new CTempCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTempCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTempCube::Clone(void * pArg)
{
	CTempCube*		pInstance = new CTempCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTempCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTempCube::Free()
{
	Super::Free();
}
