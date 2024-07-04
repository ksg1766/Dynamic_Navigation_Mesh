#include "stdafx.h"
#include "..\Public\Lava.h"
#include "GameInstance.h"

CLava::CLava(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CLava::CLava(const CLava& rhs)
	: Super(rhs)
{
}

HRESULT CLava::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLava::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	m_pNoise = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise_Liquid"), pArg));
	
	if(nullptr == m_pNoise)
		return E_FAIL;

	if (LEVEL_GAMETOOL == m_pGameInstance->GetCurrentLevelIndex())
	{
		GetRigidBody()->GetSphereCollider()->SetRadius(10.f);
		GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(5.f, 5.f, 5.f));
	}

	return S_OK;
}

void CLava::Tick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	m_UVoffset = m_UVoffset + _float2(-fTimeDelta * 0.0008f, -fTimeDelta * 0.0008f);

	if (m_UVoffset.x < 0.f)	m_UVoffset.x += 1.f;
	if (m_UVoffset.y < 0.f)	m_UVoffset.y += 1.f;

	CGameObject::Tick(fTimeDelta);
}

void CLava::LateTick(const _float& fTimeDelta)
{
	if (m_bRendered)
		return;

	CGameObject::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, this);
	m_bRendered = true;
}

void CLava::DebugRender()
{
}

HRESULT CLava::Render()
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

HRESULT CLava::RenderInstance()
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

HRESULT CLava::Ready_FixedComponents()
{
	if (FAILED(Super::Ready_FixedComponents()))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_Lava_B_emissive"))))
			return E_FAIL;

	GetShader()->SetPassIndex(3);

	return S_OK;
}

HRESULT CLava::Ready_Scripts()
{
	return S_OK;
}

HRESULT CLava::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix"))||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_EmissiveTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pNoise->Bind_ShaderResource(GetShader(), "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_UVoffset", &m_UVoffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CLava* CLava::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLava* pInstance = new CLava(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLava");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLava::Clone(void* pArg)
{
	CLava* pInstance = new CLava(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLava");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLava::Free()
{
	Super::Free();
}
