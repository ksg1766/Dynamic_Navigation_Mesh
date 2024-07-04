#include "stdafx.h"
#include "..\Public\Bubble.h"
#include "GameInstance.h"
#include "PlayerController.h"

CBubble::CBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CBubble::CBubble(const CBubble& rhs)
	: Super(rhs)
{
}

HRESULT CBubble::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBubble::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetRigidBody()->GetSphereCollider()->SetRadius(5.2f);
	GetRigidBody()->GetOBBCollider()->SetExtents(Vec3(3.f, 3.f, 3.f));

	return S_OK;
}

void CBubble::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	m_fFrameTime += fTimeDelta;

	if (m_fFrameTime > 0.2f)
		m_pGameInstance->DeleteObject(this);
}

void CBubble::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CBubble::DebugRender()
{
}

HRESULT CBubble::Render()
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

HRESULT CBubble::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;
	
	/* Com_RigidBody */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::RigidBody, TEXT("Prototype_Component_RigidDynamic")))
		|| FAILED(GetRigidBody()->InitializeCollider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBubble::Ready_Scripts()
{
	return S_OK;
}

HRESULT CBubble::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix"))||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)) ||
		FAILED(GetShader()->Bind_RawValue("g_vCamPosition", &static_cast<const _float4&>(m_pGameInstance->Get_CamPosition_Float4()), sizeof(_float4))))
	{
		return E_FAIL;
	}

	if(FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CBubble::OnCollisionEnter(CGameObject* pOther)
{
	const LAYERTAG& eLayerTag = pOther->GetLayerTag();
	if (LAYERTAG::PLAYER == eLayerTag)
	{
		CPlayerController* pPlayerController = static_cast<CPlayerController*>(pOther->GetScripts()[0]);
		pPlayerController->GetHitMessage();
	}
}

void CBubble::OnCollisionStay(CGameObject* pOther)
{
}

void CBubble::OnCollisionExit(CGameObject* pOther)
{
}

CBubble* CBubble::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBubble* pInstance = new CBubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBubble::Clone(void* pArg)
{
	CBubble* pInstance = new CBubble(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBubble::Free()
{
	Super::Free();
}
