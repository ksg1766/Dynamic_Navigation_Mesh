#include "stdafx.h"
#include "..\Public\Strife_Ammo.h"
#include "GameInstance.h"

CStrife_Ammo::CStrife_Ammo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CStrife_Ammo::CStrife_Ammo(const CStrife_Ammo& rhs)
	: Super(rhs)
{
}

HRESULT CStrife_Ammo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStrife_Ammo::Initialize(void* pArg)
{
	if (pArg) { m_tProps = *(AMMOPROPS*)pArg; }

	return S_OK;
}

void CStrife_Ammo::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
}

void CStrife_Ammo::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void CStrife_Ammo::DebugRender()
{
	Super::DebugRender();
}

HRESULT CStrife_Ammo::Render()
{
	GetShader()->Begin();

	GetBuffer()->Render();

#ifdef _DEBUG
	DebugRender();
#endif

	return S_OK;
}

HRESULT CStrife_Ammo::Ready_FixedComponents()
{
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_VtxPosTex"))))
		return E_FAIL;
	GetShader()->SetPassIndex(1);

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_Rect"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrife_Ammo::Ready_Scripts()
{
	return S_OK;
}

HRESULT CStrife_Ammo::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix")) || 
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
	{
		return E_FAIL;
	}

	/*_float4 vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightDir", &vLightDir, sizeof(_float4))))
		return E_FAIL;

	_float4	vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightDiffuse", &vDiffuse, sizeof(_float4))))
		return E_FAIL;

	_float4 vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightAmbient", &vAmbient, sizeof(_float4))))
		return E_FAIL;

	_float4 vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(GetShader()->Bind_RawValue("g_vLightSpecular", &vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

void CStrife_Ammo::OnCollisionEnter(CGameObject* pOther)
{
}

void CStrife_Ammo::OnCollisionStay(CGameObject* pOther)
{
}

void CStrife_Ammo::OnCollisionExit(CGameObject* pOther)
{
}

_bool CStrife_Ammo::LifeTime(const _float& fTimeDelta)
{
	m_tProps.fLifeTime -= fTimeDelta;

	if (m_tProps.fLifeTime < 0.f)
	{
		m_pGameInstance->DeleteObject(this);
		return false;
	}
	return true;
}

void CStrife_Ammo::Move(const _float& fTimeDelta)
{
	static_cast<CRigidDynamic*>(GetRigidBody())->AddForce(m_tProps.vVelocity, ForceMode::VELOCITY_CHANGE);

	(m_tProps.iKeyFrame >= m_tProps.iMaxKeyFrame) ? m_tProps.iKeyFrame = 0 : ++m_tProps.iKeyFrame;
}

void CStrife_Ammo::Free()
{
	Super::Free();
}
