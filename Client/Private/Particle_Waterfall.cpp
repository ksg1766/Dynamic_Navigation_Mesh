#include "stdafx.h"
#include "..\Public\Particle_Waterfall.h"
#include "GameInstance.h"
#include "VIBuffer_ParticleSystem.h"
#include "Shader.h"
#include "Utils.h"

CParticle_Waterfall::CParticle_Waterfall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CParticle_Waterfall::CParticle_Waterfall(const CParticle_Waterfall& rhs)
	: Super(rhs)
{
}

HRESULT CParticle_Waterfall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Waterfall::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Waterfall::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);

	if (m_fMaxLifeTime > 0.f && m_fMaxLifeTime < m_fLifeTime)
		m_pGameInstance->DeleteObject(this);

	m_fTimeDelta = fTimeDelta;
	m_fLifeTime += m_fTimeDelta;
}

void CParticle_Waterfall::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CParticle_Waterfall::DebugRender()
{
	Super::DebugRender();
}

HRESULT CParticle_Waterfall::Render()
{
	if (nullptr == GetBuffer() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

#ifdef _DEBUG
	DebugRender();
#endif

	if (FAILED(static_cast<CVIBuffer_ParticleSystem*>(GetBuffer())->Render(m_pShaderStream, GetShader())))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Waterfall::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_ParticleSystem_Draw"))))
		return E_FAIL;
	GetShader()->SetPassIndex(2);
	m_pShaderStream = static_cast<CShader*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleSystem_Stream")));
	if (nullptr == m_pShaderStream)
		return E_FAIL;
	m_pShaderStream->SetPassIndex(2);

	/* Com_VIBuffer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Buffer, TEXT("Prototype_Component_VIBuffer_ParticleSystem"))))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Super::AddComponent(LEVEL_GAMEPLAY, ComponentType::Texture, TEXT("Prototype_Component_Texture_WaterfallSplash"))))
		return E_FAIL;

	Utils::CreateRandomTexture1DSRV(m_pDevice, &m_pRandomTexture);
	if (nullptr == m_pRandomTexture)
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Waterfall::Ready_Scripts()
{
	if (LEVEL_GAMEPLAY == m_pGameInstance->GetCurrentLevelIndex())
	{

	}

	return S_OK;
}

HRESULT CParticle_Waterfall::Bind_ShaderResources()
{
	const Matrix& matView = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
	const Matrix& matProj = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	Matrix matViewProj = matView * matProj;

	if (/*FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||*/
		FAILED(GetShader()->Bind_Matrix("g_ViewProj", &matViewProj)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
		return E_FAIL;

	_float3 vGravity = { 0.0f, -9.81f, 0.0f };
	Vec3 vEmitPosition = GetTransform()->GetPosition();
	if (FAILED(m_pShaderStream->Bind_RawValue("g_vEmitPosition", &vEmitPosition, sizeof(_float3))) ||
		FAILED(m_pShaderStream->Bind_RawValue("g_vEmitDirection", &m_vEmitDirection, sizeof(_float3))) ||
		FAILED(m_pShaderStream->Bind_RawValue("g_fGameTime", &m_fLifeTime, sizeof(_float))) ||
		FAILED(m_pShaderStream->Bind_RawValue("g_fTimeStep", &m_fTimeDelta, sizeof(_float))) ||
		FAILED(GetShader()->Bind_RawValue("g_WaveSplashAcc", &vGravity, sizeof(_float3))))
		return E_FAIL;
	//g_WaveSplashAcc // 중력은 여기다 넣어줘야 재활용할 수 있을 듯

	if (FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", 0)) ||
		//FAILED(m_pTextureEx->Bind_ShaderResource(GetShader(), "g_Texture", 0)) ||
		FAILED(m_pShaderStream->Bind_Texture("g_RandomTexture", m_pRandomTexture)))
		return E_FAIL;

	return S_OK;
}

CParticle_Waterfall* CParticle_Waterfall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Waterfall* pInstance = new CParticle_Waterfall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Waterfall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Waterfall::Clone(void* pArg)
{
	CParticle_Waterfall* pInstance = new CParticle_Waterfall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Waterfall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Waterfall::Free()
{
	Safe_Release(m_pShaderStream);
	Safe_Release(m_pRandomTexture);
	Super::Free();
}
