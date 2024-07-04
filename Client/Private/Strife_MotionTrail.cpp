#include "stdafx.h"
#include "..\Public\Strife_MotionTrail.h"
#include "GameInstance.h"

CStrife_MotionTrail::CStrife_MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_MotionTrail::CStrife_MotionTrail(const CStrife_MotionTrail& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_MotionTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_MotionTrail::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	MOTIONTRAIL_DESC pDesc = *reinterpret_cast<MOTIONTRAIL_DESC*>(pArg);
	m_pModel = pDesc.pModel;
	m_tTweendesc = *pDesc.pTweendesc;
	GetTransform()->Set_WorldMatrix(pDesc.matWorld);
	m_fLifeTime = pDesc.fLifeTime;

	return S_OK;
}

void CStrife_MotionTrail::Tick(const _float& fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime < 0.f)
	{
		m_pGameInstance->DeleteObject(this);
		return;
	}

	Super::Tick(fTimeDelta);
}

void CStrife_MotionTrail::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CStrife_MotionTrail::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_MotionTrail::Render()
{
	if (nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	if (FAILED(GetShader()->Bind_Texture("g_TransformMap", m_pModel->GetSRV())))
		return E_FAIL;

	/* 본의 최종 트랜스폼 계산 : <오프셋 * 루트 기준 * 사전변환> */
	if (FAILED(GetShader()->Bind_RawValue("g_Tweenframes", &m_tTweendesc, sizeof(TWEENDESC))))
		return E_FAIL;

	for (_uint i = 0; i < m_pModel->GetNumMeshes(); i++)
	{
		m_pModel->BindMaterialTexture(GetShader(), "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModel->BindMaterialTexture(GetShader(), "g_NormalTexture", i, aiTextureType_NORMALS);

		if (FAILED(GetShader()->Begin()))
			return E_FAIL;

		if (FAILED(m_pModel->GetMeshes()[i]->Render()))
			return E_FAIL;
	}

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CStrife_MotionTrail::Ready_FixedComponents()
{
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxTexFetchAnim"))))
		return E_FAIL;
	GetShader()->SetPassIndex(3);

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{

	}

	return S_OK;
}

HRESULT CStrife_MotionTrail::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{

	}

	return S_OK;
}

HRESULT CStrife_MotionTrail::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	Color vColorOffset = Color(0.0f, 0.f, 0.24f, 0.f);
	GetShader()->Bind_RawValue("g_ColorOffset", &vColorOffset, sizeof(Color));

	return S_OK;
}

CStrife_MotionTrail* CStrife_MotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStrife_MotionTrail* pInstance = new CStrife_MotionTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStrife_MotionTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStrife_MotionTrail::Clone(void* pArg)
{
	CStrife_MotionTrail* pInstance = new CStrife_MotionTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStrife_MotionTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStrife_MotionTrail::Free()
{
	Super::Free();
}
