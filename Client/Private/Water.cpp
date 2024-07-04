#include "stdafx.h"
#include "..\Public\Water.h"
#include "GameInstance.h"

CWater::CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CWater::CWater(const CWater& rhs)
	: Super(rhs)
{
}

HRESULT CWater::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWater::Initialize(void* pArg)
{
	WATER_DESC* pDesc = reinterpret_cast<WATER_DESC*>(pArg);

	if (FAILED(Ready_FixedComponents(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetTransform()->Translate(pDesc->vPos);

	return S_OK;
}

void CWater::Tick(const _float& fTimeDelta)
{
	m_fWaterTranslation += _float(-fTimeDelta * m_fWaterTranslationSpeed);

	if (m_fWaterTranslation < 0.f)	m_fWaterTranslation += 1.f;

	Super::Tick(fTimeDelta);

	if (WaterLevelMode::Dagon == m_eMode)
	{
		Dagon(fTimeDelta);
	}
	else if(WaterLevelMode::Dessert == m_eMode)
	{
		Desert(fTimeDelta);
	}
}

void CWater::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_WATER, this);
}

void CWater::DebugRender()
{
}

HRESULT CWater::Render()
{
	if (nullptr == GetFixedComponent(ComponentType::Terrain) || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	GetShader()->Begin();

	static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->Render();

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

void CWater::Dagon(const _float& fTimeDelta)
{
	const Vec3& vPos = GetTransform()->GetPosition();
	if (vPos.y > 28.25f)
	{
		Vec3 vEndPos = vPos;
		vEndPos.y = 28.25f;
		GetTransform()->SetPosition(vEndPos);
		m_eMode = WaterLevelMode::End;
		return;
	}
	m_bFresnel = false;
	m_iCurrentNormalMap = 0;
	GetTransform()->Translate(2.f * fTimeDelta * Vec3::UnitY);
}

void CWater::Desert(const _float& fTimeDelta)
{
	const Vec3& vPos = GetTransform()->GetPosition();
	if (vPos.y < 1.f)
	{
		Vec3 vEndPos = vPos;
		vEndPos.y = 0.f;
		GetTransform()->SetPosition(vEndPos);
		m_eMode = WaterLevelMode::End;
		m_iCurrentNormalMap = 1;
		m_bFresnel = true;
		return;
	}
	GetTransform()->Translate(3.f * -fTimeDelta * Vec3::UnitY);
}

HRESULT CWater::Ready_FixedComponents(WATER_DESC* pDesc)
{
	// 터레인 필요.
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;
	/* Com_Terrain */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Terrain, TEXT("Prototype_Component_Terrain"))) ||
		FAILED(static_cast<CTerrain*>(GetFixedComponent(ComponentType::Terrain))->InitializeNorTex(pDesc->vSize.x, pDesc->vSize.y, pDesc->vSize.x, pDesc->vSize.y)))
		return E_FAIL;
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxPosTex"))))
		return E_FAIL;
	GetShader()->SetPassIndex(3);

	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_Water_Normal"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Ready_Scripts()
{
	return S_OK;
}

HRESULT CWater::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (KEY_PRESSING(KEY::CTRL) && KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::N))
	{
		++m_iCurrentNormalMap;
		m_iCurrentNormalMap %= 3;
	}

	if (KEY_PRESSING(KEY::CTRL) && KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::O))
		m_fWaterTranslationSpeed -= 0.001f;
	if (KEY_PRESSING(KEY::CTRL) && KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::P))
		m_fWaterTranslationSpeed += 0.001f;

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_NormalTexture", m_iCurrentNormalMap)))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_fWaterTranslation", &m_fWaterTranslation, sizeof(_float))))
		return E_FAIL;
	if (FAILED(GetShader()->Bind_RawValue("g_fReflectRefractScale", &m_fReflectRefractScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(GetShader()->Bind_RawValue("g_bFresnel", &m_bFresnel, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CWater* CWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWater* pInstance = new CWater(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWater");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWater::Clone(void* pArg)
{
	CWater* pInstance = new CWater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWater");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWater::Free()
{
	Super::Free();
}
