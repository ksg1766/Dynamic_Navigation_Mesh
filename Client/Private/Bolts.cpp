#include "stdafx.h"
#include "..\Public\Bolts.h"
#include "GameInstance.h"

CBolts::CBolts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CBolts::CBolts(const CBolts& rhs)
	: Super(rhs)
{
}

HRESULT CBolts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBolts::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CBolts::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	m_fFrameTime += fTimeDelta;

	if (m_fFrameTime > 0.25f)
		m_pGameInstance->DeleteObject(this);

	float fScale = 47.f * fTimeDelta;

	m_vScale.x += fScale;
	m_vScale.y += 3.f * fScale;
	m_vScale.z += fScale;

	GetTransform()->SetScale(m_vScale);
}

void CBolts::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CBolts::DebugRender()
{
}

HRESULT CBolts::Render()
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

HRESULT CBolts::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(2);
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

HRESULT CBolts::Ready_Scripts()
{
	return S_OK;
}

HRESULT CBolts::Bind_ShaderResources()
{
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	if(FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CBolts* CBolts::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBolts* pInstance = new CBolts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBolts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBolts::Clone(void* pArg)
{
	CBolts* pInstance = new CBolts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBolts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBolts::Free()
{
	Super::Free();
}
