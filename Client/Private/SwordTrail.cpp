#include "stdafx.h"
#include "..\Public\SwordTrail.h"
#include "GameInstance.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& rhs)
	: Super(rhs)
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwordTrail::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	SWORDTRAIL_DESC* pDesc = reinterpret_cast<SWORDTRAIL_DESC*>(pArg);

	m_pModel = pDesc->pModel;
	//GetTransform()->SetScale(Vec3(10.f, 10.f, 10.f));

	m_matOffsetTop = pDesc->matOffsetTop;
	m_iTextureIndex = pDesc->iIndex;

	return S_OK;
}

void CSwordTrail::Tick(const _float& fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	m_fTimeDelta = fTimeDelta;

	/*m_vDistortionOffset.y += fTimeDelta * 1.f;
	if (m_vDistortionOffset.y > 1.f)
		m_vDistortionOffset.y = 0.f;*/

	Super::Tick(fTimeDelta);
}

void CSwordTrail::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CSwordTrail::DebugRender()
{
	Super::DebugRender();
}

HRESULT CSwordTrail::Render()
{
	/*GetTransform()->Set_WorldMatrix(m_pModel->GetGameObject()->GetTransform()->WorldMatrix());
	m_tTweenDesc = m_pModel->GetTweenDesc();*/

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

HRESULT CSwordTrail::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_ParticleSystem_Draw"))))
		return E_FAIL;
	GetShader()->SetPassIndex(3);

	m_pShaderStream = static_cast<CShader*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleSystem_Stream")));
	if (nullptr == m_pShaderStream)
		return E_FAIL;
	m_pShaderStream->SetPassIndex(3);

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
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Texture, TEXT("Prototype_Component_Texture_Fire_Tiled"))))
		return E_FAIL;

	//m_pMaskTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fire_SubUV")));
	m_pMaskTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(this, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlameMask")));
	if (!m_pMaskTexture) return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Ready_Scripts()
{


	return S_OK;
}

HRESULT CSwordTrail::Bind_ShaderResources()
{
	const Matrix& matView = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
	const Matrix& matProj = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	Matrix matViewProj = matView * matProj;

	if (//FAILED(GetTransform()->Bind_ShaderResources(m_pShaderStream, "g_WorldMatrix")) ||
		FAILED(m_pModel->GetTransform()->Bind_ShaderResources(m_pShaderStream, "g_WorldMatrix")) ||
		FAILED(GetShader()->Bind_Matrix("g_ViewProj", &matViewProj)) ||
		FAILED(GetTexture()->Bind_ShaderResource(GetShader(), "g_Texture", 0)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))) ||
		FAILED(m_pMaskTexture->Bind_ShaderResource(GetShader(), "g_MaskTexture", m_iTextureIndex)))
		return E_FAIL;

	Vec3 vEmitDirection = Vec3::UnitY;
	if (FAILED(m_pShaderStream->Bind_RawValue("g_vEmitDirection", &vEmitDirection, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderStream->Bind_RawValue("g_Tweenframes", &m_pModel->GetTweenDesc(), sizeof(TWEENDESC))))
		return E_FAIL;
	if (FAILED(m_pShaderStream->Bind_Texture("g_TransformMap", m_pModel->GetSRV())))
		return E_FAIL;
	if (FAILED(m_pShaderStream->Bind_Matrix("g_matOffsetTop", &m_matOffsetTop)))
		return E_FAIL;
	if (FAILED(m_pShaderStream->Bind_RawValue("g_iSocketBoneIndex", &m_pModel->GetSocketBoneIndex(), sizeof(_int))))
		return E_FAIL;
	_float fTimeStep = (rand() % 4) * m_fTimeDelta;
	if (FAILED(m_pShaderStream->Bind_RawValue("g_fTimeStep", &fTimeStep, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderStream->Bind_RawValue("g_fGameTime", &m_fLifeTime, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordTrail::Clone(void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrail::Free()
{
	Super::Free();
}
