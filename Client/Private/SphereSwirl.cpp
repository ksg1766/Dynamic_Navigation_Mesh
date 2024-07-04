#include "stdafx.h"
#include "..\Public\SphereSwirl.h"
#include "GameInstance.h"

CSphereSwirl::CSphereSwirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSphereSwirl::CSphereSwirl(const CSphereSwirl& rhs)
	: Super(rhs)
{
}

HRESULT CSphereSwirl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSphereSwirl::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CSphereSwirl::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	m_fFrameTime += fTimeDelta;

	if (m_fFrameTime > 15.f)
		m_pGameInstance->DeleteObject(this);

	GetTransform()->Rotate(Vec3(1.7f, 1.f, 1.f));
}

void CSphereSwirl::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CSphereSwirl::DebugRender()
{
}

HRESULT CSphereSwirl::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT CSphereSwirl::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(6);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphereSwirl::Ready_Scripts()
{
	return S_OK;
}

HRESULT CSphereSwirl::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix"))||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	if(FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CSphereSwirl* CSphereSwirl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphereSwirl* pInstance = new CSphereSwirl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSphereSwirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSphereSwirl::Clone(void* pArg)
{
	CSphereSwirl* pInstance = new CSphereSwirl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSphereSwirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSphereSwirl::Free()
{
	Super::Free();
}
