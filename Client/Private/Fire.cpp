#include "stdafx.h"
#include "..\Public\Fire.h"
#include "GameInstance.h"
#include "Layer.h"

CFire::CFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CFire::CFire(const CFire& rhs)
	: Super(rhs)
{
}

HRESULT CFire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFire::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts(pArg)))
		return E_FAIL;
	
	//GetTransform()->SetScale(Vec3(10.f, 10.f, 10.f));
	//GetTransform()->Rotate(Vec3(90.f, 0.0f, 0.f));

	return S_OK;
}

void CFire::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 1000.0f)
	{
		m_fFrameTime = 0.f;
	}
}

void CFire::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CFire::DebugRender()
{
	Super::DebugRender();
}

HRESULT CFire::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetShader()->Begin()))
		return E_FAIL;

	if(FAILED(GetBuffer()->Render()))
		return E_FAIL;


#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CFire::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Fire"))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Point"))))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;
	
	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_Fire_Tiled"))))
		return E_FAIL;

	m_pNoiseTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke_Tiled"), nullptr));
	m_pAlphaTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_EggMask"), nullptr));
	if (!m_pNoiseTexture || !m_pAlphaTexture)
		return E_FAIL;

	return S_OK;
}

HRESULT CFire::Ready_Scripts(void* pArg)
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CFire::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_FireTexture", 0)) ||
		FAILED(m_pNoiseTexture->Bind_ShaderResource(GetShader(), "g_NoiseTexture", 0)) ||
		FAILED(m_pAlphaTexture->Bind_ShaderResource(GetShader(), "g_AlphaTexture", 0)) ||
		FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))) ||
		FAILED(GetShader()->Bind_Matrix("g_matOffset", &m_matPivot)))
		return E_FAIL;

	return S_OK;
}

CFire* CFire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFire* pInstance = new CFire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFire::Clone(void* pArg)
{
	CFire* pInstance = new CFire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire::Free()
{
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
	Super::Free();
}
