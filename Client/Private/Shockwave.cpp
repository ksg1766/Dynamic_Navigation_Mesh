#include "stdafx.h"
#include "..\Public\Shockwave.h"
#include "GameInstance.h"
#include "Layer.h"

CShockwave::CShockwave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CShockwave::CShockwave(const CShockwave& rhs)
	: Super(rhs)
{
}

HRESULT CShockwave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShockwave::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CShockwave::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	m_fWrapTime -= fTimeDelta;

	if (m_fFrameTime >= 3.f)
	{
		m_pGameInstance->DeleteObject(this);
		return;
	}

	if (m_fWrapTime <= 0.f)
		m_fWrapTime = 0.2f;

	_float fScale = 2100.f * m_fWrapTime;
	GetTransform()->SetScale(Vec3(fScale, fScale, fScale));
}

void CShockwave::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CShockwave::DebugRender()
{
	Super::DebugRender();
}

HRESULT CShockwave::Render()
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

HRESULT CShockwave::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(7);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_Sun"))))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShockwave::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CShockwave::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CShockwave* CShockwave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShockwave* pInstance = new CShockwave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShockwave::Clone(void* pArg)
{
	CShockwave* pInstance = new CShockwave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShockwave::Free()
{
	Super::Free();
}
