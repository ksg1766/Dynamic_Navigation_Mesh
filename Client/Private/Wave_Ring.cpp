#include "stdafx.h"
#include "..\Public\Wave_Ring.h"
#include "GameInstance.h"

CWave_Ring::CWave_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CWave_Ring::CWave_Ring(const CWave_Ring& rhs)
	: Super(rhs)
{
}

HRESULT CWave_Ring::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWave_Ring::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;
	
	GetTransform()->SetScale(Vec3(1.5f, 1.2f, 1.5f));
	GetTransform()->Translate(Vec3(0.f, 0.5f, 0.f));
	//GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	return S_OK;
}

void CWave_Ring::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.27f)
	{
		m_pGameInstance->DeleteObject(this);
	}
}

void CWave_Ring::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CWave_Ring::DebugRender()
{
	Super::DebugRender();
}

HRESULT CWave_Ring::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CWave_Ring::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(1);

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

HRESULT CWave_Ring::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CWave_Ring::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CWave_Ring* CWave_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWave_Ring* pInstance = new CWave_Ring(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWave_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWave_Ring::Clone(void* pArg)
{
	CWave_Ring* pInstance = new CWave_Ring(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWave_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWave_Ring::Free()
{
	Super::Free();
}
