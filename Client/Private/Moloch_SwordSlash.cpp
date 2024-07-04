#include "stdafx.h"
#include "..\Public\Moloch_SwordSlash.h"
#include "GameInstance.h"
#include "VIBuffer_Point_Instance.h"

CMoloch_SwordSlash::CMoloch_SwordSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CMoloch_SwordSlash::CMoloch_SwordSlash(const CMoloch_SwordSlash& rhs)
	: Super(rhs)
{
}

HRESULT CMoloch_SwordSlash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoloch_SwordSlash::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
		EFFECT_DESC pDesc = *reinterpret_cast<EFFECT_DESC*>(pArg);
		m_fLifeTime = pDesc.fLifeTime;
	}

	return S_OK;
}

void CMoloch_SwordSlash::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	m_fLifeTime -= fTimeDelta;

	if(m_fLifeTime < 0.f)
		m_pGameInstance->DeleteObject(this);

	if (!m_bCW)
	{
		m_UVoffset.x -= fTimeDelta;
		m_UVoffset.y -= fTimeDelta;
	}
	else
	{
		m_UVoffset.x += fTimeDelta;
		m_UVoffset.y += fTimeDelta;
	}
}

void CMoloch_SwordSlash::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CMoloch_SwordSlash::DebugRender()
{
	Super::DebugRender();
}

HRESULT CMoloch_SwordSlash::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetModel()->Render();

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CMoloch_SwordSlash::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxMesh"))))
		return E_FAIL;
	GetShader()->SetPassIndex(3);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_RigidBody */
	/*if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;*/

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	m_pNoiseTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_RGB")));
	m_pMaskTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask_Swipe")));

	return S_OK;
}

HRESULT CMoloch_SwordSlash::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{
	}

	return S_OK;
}

HRESULT CMoloch_SwordSlash::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) || 
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTexture->Bind_ShaderResource(GetShader(), "g_NoiseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pMaskTexture->Bind_ShaderResource(GetShader(), "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &m_UVoffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CMoloch_SwordSlash* CMoloch_SwordSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoloch_SwordSlash* pInstance = new CMoloch_SwordSlash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMoloch_SwordSlash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoloch_SwordSlash::Clone(void* pArg)
{
	CMoloch_SwordSlash* pInstance = new CMoloch_SwordSlash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMoloch_SwordSlash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoloch_SwordSlash::Free()
{
	Super::Free();
}
