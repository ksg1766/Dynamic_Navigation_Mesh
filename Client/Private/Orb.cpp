#include "stdafx.h"
#include "..\Public\Orb.h"
#include "GameInstance.h"
#include "SphereSwirl.h"

#include "Strife_Ammo_Default.h"
#include "Layer.h"

COrb::COrb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

COrb::COrb(const COrb& rhs)
	: Super(rhs)
{
}

HRESULT COrb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COrb::Initialize(void* pArg)
{
	if (FAILED(Ready_FixedComponents()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	GetTransform()->SetScale(Vec3(4.f, 4.f, 4.f));

	return S_OK;
}

void COrb::Tick(const _float& fTimeDelta)
{
	Super::Tick(fTimeDelta);
	m_fFrameTime += fTimeDelta;

	if (!m_bSpark)
	{
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Lightning_Spark"), LAYERTAG::IGNORECOLLISION)->GetTransform()->Translate(GetTransform()->GetPosition() - 2.f * Vec3::UnitY);

		m_pSphereSwirl = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_SphereSwirl"), LAYERTAG::IGNORECOLLISION);
		m_pSphereSwirl->GetTransform()->Translate(GetTransform()->GetPosition());

		m_bSpark = true;
	}

	if (m_fFrameTime < 0.4f)
	{
		GetTransform()->SetScale(Vec3(10.f * m_fFrameTime, 10.f * m_fFrameTime, 10.f * m_fFrameTime));
		m_pSphereSwirl->GetTransform()->SetScale(Vec3(10.f * m_fFrameTime, 10.f * m_fFrameTime, 10.f * m_fFrameTime));
	}
	else
	{
		if (!m_bSpawned)
		{
			GetTransform()->SetScale(Vec3(4.f, 4.f, 4.f));
			m_pSphereSwirl->GetTransform()->SetScale(Vec3(4.f, 4.f, 4.f));
			m_bSpawned = true;
		}
	}

	if (!m_bCharged &&m_fFrameTime > 6.f)
	{
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_orb_projectile_build.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.7f)))
			__debugbreak();

		CGameObject* pEffect = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Shockwave"), LAYERTAG::IGNORECOLLISION);
		pEffect->GetTransform()->SetPosition(GetTransform()->GetPosition());

		m_bCharged = true;
	}
	if (!m_bLaunched && m_fFrameTime > 9.f)
	{
		Fire();
		if (FAILED(m_pGameInstance->PlaySoundFile(TEXT("en_waterboss_orb_projectile.ogg"), CHANNELID::CHANNEL_ENEMY2, 0.7f)))
			__debugbreak();

		m_bLaunched = true;
	}
	
	if (m_fFrameTime > 10.f)
	{
		GetTransform()->SetScale(GetTransform()->GetLocalScale() * (1.f - fTimeDelta));
		m_pSphereSwirl->GetTransform()->SetScale(GetTransform()->GetLocalScale() * (1.f - fTimeDelta));
	}

	if (m_fFrameTime > 12.5f)
	{
		m_pGameInstance->DeleteObject(this);
		m_pGameInstance->DeleteObject(m_pSphereSwirl);
	}
}

void COrb::LateTick(const _float& fTimeDelta)
{
	Super::LateTick(fTimeDelta);

	GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

void COrb::DebugRender()
{
}

HRESULT COrb::Render()
{
	if (nullptr == GetModel() || nullptr == GetShader())
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(GetModel()->Render()))
		return E_FAIL;

	if (m_fFrameTime < 0.4f)
	{
		GetShader()->SetPassIndex(5);
		if (FAILED(m_pWispModel->Render()))
			return E_FAIL;

		/*GetShader()->SetPassIndex(3);
		for (_int i = 0; i < 4; ++i)
		{
			if (FAILED(m_pSparkModels[i]->Render()))
				return E_FAIL;

			GetTransform()->RotateYAxisFixed(Vec3(0.f, 90.f, 0.f));
		}*/
		GetShader()->SetPassIndex(4);
	}

#ifdef _DEBUG
	Super::DebugRender();
#endif

	return S_OK;
}

HRESULT COrb::Ready_FixedComponents()
{
	/* Com_Shader */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Shader, TEXT("Prototype_Component_Shader_Sphere"))))
		return E_FAIL;
	GetShader()->SetPassIndex(4);

	/* Com_Model */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Model, TEXT("Prototype_Component_Model_") + GetObjectTag())))
		return E_FAIL;

	m_pWispModel = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Wisp")));
	/*m_pSparkModels[0] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark0")));
	m_pSparkModels[1] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark1")));
	m_pSparkModels[2] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark2")));
	m_pSparkModels[3] = static_cast<CModel*>(m_pGameInstance->Clone_Component(this, LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning_Spark3")));*/

	/* Com_Transform */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Transform, TEXT("Prototype_Component_Transform"))))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(Super::AddComponent(LEVEL_STATIC, ComponentType::Renderer, TEXT("Prototype_Component_Renderer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT COrb::Ready_Scripts()
{
	return S_OK;
}

HRESULT COrb::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	if (FAILED(GetTransform()->Bind_ShaderResources(GetShader(), "g_WorldMatrix"))||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ViewMatrix", CPipeLine::D3DTS_VIEW)) ||
		FAILED(m_pGameInstance->Bind_TransformToShader(GetShader(), "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	if(FAILED(GetShader()->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void COrb::Fire()
{
	CGameObject* pAmmo = nullptr;
	CStrife_Ammo::AMMOPROPS tProps;

	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	auto iter = mapLayers.find(LAYERTAG::PLAYER);
	if (iter == mapLayers.end())
		return;

	CTransform* pTargetTransform = iter->second->GetGameObjects().front()->GetTransform();

	Vec3 vTargetPosition = pTargetTransform->GetPosition();
	Vec3 vFireDirection = vTargetPosition - GetTransform()->GetPosition();
	vFireDirection.Normalize();

	tProps = { CStrife_Ammo::AmmoType::DEFAULT, 7, 0, 50, 30.f * vFireDirection, false, 3.f };
	pAmmo = m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Strife_Ammo_Default"), LAYERTAG::UNIT_AIR, &tProps);

	CTransform* pAmmoTransform = pAmmo->GetTransform();
	pAmmoTransform->GetTransform()->Rotate(vFireDirection);
	pAmmoTransform->GetTransform()->Rotate(Vec3(0.f, 180.f, 0.f));
	
	//Matrix& matWorld = pAmmoTransform->WorldMatrix();

	//_float fRight = matWorld.Right().Length();
	//_float fUp = matWorld.Up().Length();
	//_float fLook = matWorld.Backward().Length();

	//matWorld.Backward(/*fLook * */vFireDirection);
	//Vec3 vRight = Vec3::UnitY.Cross(vFireDirection);
	//vRight.Normalize();
	//matWorld.Right(/*fRight * */vRight);
	//Vec3 vUp = vFireDirection.Cross(vRight);
	//vUp.Normalize();
	//matWorld.Up(/*fUp * */vUp);

	pAmmoTransform->SetScale(Vec3(2.7f, 5.f, 1.f));/*
	pAmmoTransform->Rotate(Vec3(90.f, 0.f, 0.f));
	pAmmoTransform->Rotate(Vec3(0.f, 180.f, 0.f));*/

	//pAmmoTransform->Rotate(qRot);
	//pAmmoTransform->RotateYAxisFixed(vRotateAmount);
	pAmmoTransform->SetPosition(GetTransform()->GetPosition());

	static_cast<CStrife_Ammo_Default*>(pAmmo)->SetColor(Color(0.135f, 0.13f, 1.0f));
}

COrb* COrb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COrb* pInstance = new COrb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : COrb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COrb::Clone(void* pArg)
{
	COrb* pInstance = new COrb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : COrb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COrb::Free()
{
	Safe_Release(m_pWispModel);
	for(_int i = 0; i < 4; ++i)
		Safe_Release(m_pSparkModels[i]);

	Super::Free();
}
