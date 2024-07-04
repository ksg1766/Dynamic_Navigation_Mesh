#include "stdafx.h"
#include "ParticleController.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "DebugDraw.h"

CParticleController::CParticleController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

CParticleController::CParticleController(const CParticleController& rhs)
	:Super(rhs)
{
}

HRESULT CParticleController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleController::Initialize(void* pArg)
{
	m_pTransform = GetTransform();
	if (m_pGameObject->GetRigidBody())
	{
		m_pRigidBody = static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody());
		m_pRigidBody->SetMass(0.25f);
		m_pRigidBody->SetMaterialDrag(0.000001f);
		m_pRigidBody->SetMaterialAngularDrag(0.f);
	}

	if (nullptr == pArg)
		return E_FAIL;

	m_tProps = *reinterpret_cast<PARTICLE_DESC*>(pArg);

	random_device		RandomDevice;

	mt19937_64							RandomNumber(RandomDevice());
	uniform_real_distribution<_float>	RandomX(m_tProps.vRange.x * -0.5f, m_tProps.vRange.x * 0.5f);
	uniform_real_distribution<_float>	RandomY(m_tProps.vRange.y * -0.5f, m_tProps.vRange.y * 0.5f);
	uniform_real_distribution<_float>	RandomZ(m_tProps.vRange.z * -0.5f, m_tProps.vRange.z * 0.5f);

	uniform_real_distribution<_float>	RandomSpeedX(m_tProps.vSpeedMin.x, m_tProps.vSpeedMax.x);
	uniform_real_distribution<_float>	RandomSpeedY(m_tProps.vSpeedMin.y, m_tProps.vSpeedMax.y);
	uniform_real_distribution<_float>	RandomSpeedZ(m_tProps.vSpeedMin.z, m_tProps.vSpeedMax.z);
	uniform_real_distribution<_float>	RandomLifeTime(m_tProps.fLifeTimeMin, m_tProps.fLifeTimeMax);

	uniform_real_distribution<_float>	RandomScale(m_tProps.fScaleMin, m_tProps.fScaleMax);

	_float		fScale = 2.f * RandomScale(RandomNumber);

	m_vSpeed.x = RandomSpeedX(RandomDevice);
	m_vSpeed.y = RandomSpeedY(RandomDevice);
	m_vSpeed.z = RandomSpeedZ(RandomDevice);
	m_fLifeTIme = RandomLifeTime(RandomDevice);

	m_pTransform->SetScale(Vec3(fScale));
	m_pTransform->SetPosition(Vec3(m_tProps.vCenter.x + RandomX(RandomNumber), 2.f + m_tProps.vCenter.y + RandomY(RandomNumber), m_tProps.vCenter.z + RandomZ(RandomNumber)));

	if (m_pRigidBody)
	{
		m_pRigidBody->AddForce(m_vSpeed, ForceMode::IMPULSE);
		m_vSpeed.x *= 2.f;
		m_vSpeed.z *= 2.f;
		m_pRigidBody->AddTorque(70.f * m_vSpeed, ForceMode::IMPULSE);
	}
	//m_pTransform->WorldMatrix().Backward(m_vSpeed);

	//m_pRigidBody->AddForce(m_vSpeed, ForceMode::IMPULSE);
	//m_pRigidBody->SetMaterialDrag(0.0f);

	return S_OK;
}

void CParticleController::Tick(const _float& fTimeDelta)
{
	m_fLifeTIme -= fTimeDelta;
	
	if (m_fLifeTIme > 0.f)
	{
		switch (m_tProps.eType)
		{
		case ParticleType::FLY:
			RandomFly(fTimeDelta);
			break;
		case ParticleType::FLOAT:
			RandomFloat(fTimeDelta);
			break;
		case ParticleType::EXPLODE:
			RandomExplode(fTimeDelta);
			break;
		case ParticleType::RIGIDBODY:
			//RigidExplode(fTimeDelta);
			break;
		}

		const Vec3& vRight = m_pGameObject->GetTransform()->GetRight();
		const Vec3& vUp = m_pGameObject->GetTransform()->GetUp();

		Vec3 vNewRight = (1.f - 0.7f * fTimeDelta) * vRight;
		Vec3 vNewUp = (1.f - 0.7f * fTimeDelta) * vUp;

		m_pGameObject->GetTransform()->SetRight(vNewRight);
		m_pGameObject->GetTransform()->SetUp(vNewUp);
	}
	else
	{
		m_pGameInstance->DeleteObject(m_pGameObject);
	}
}

void CParticleController::LateTick(const _float& fTimeDelta)
{
	//m_pGameObject->GetShader()->Bind_RawValue("g_vMtrlEmissive", &m_tProps.vColor, sizeof(Color));
}

void CParticleController::DebugRender()
{
}

void CParticleController::RandomFly(const _float& fTimeDelta)
{
	const Vec3& vSpeed = m_tProps.vSpeedMin;

	m_vSpeed.x -= vSpeed.x * fTimeDelta;
	m_vSpeed.y -= vSpeed.y * fTimeDelta;
	m_vSpeed.z -= vSpeed.z * fTimeDelta;
	m_pTransform->Translate(m_vSpeed * fTimeDelta);
}

void CParticleController::RandomFloat(const _float& fTimeDelta)
{
	m_vSpeed.x *= (1.f - fTimeDelta);
	m_vSpeed.y *= (1.f - fTimeDelta);
	m_vSpeed.y += 2.f * fTimeDelta;
	m_vSpeed.z *= (1.f - fTimeDelta);
	m_pTransform->Translate(m_vSpeed * fTimeDelta);
}

void CParticleController::RandomExplode(const _float& fTimeDelta)
{
	m_vSpeed.y -= 0.07f;
	m_pTransform->Translate(m_vSpeed * fTimeDelta);
}

void CParticleController::RigidExplode(const _float& fTimeDelta)
{
}

CParticleController* CParticleController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleController* pInstance = new CParticleController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticleController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CParticleController::Clone(CGameObject* pGameObject, void* pArg)
{
	CParticleController* pInstance = new CParticleController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticleController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleController::Free()
{
	Super::Free();
}
