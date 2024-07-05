#include "ComponentManager.h"
#include "LevelManager.h"
#include "GameObject.h"

CRigidDynamic::CRigidDynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, RigidBodyType::DYNAMIC)
	, m_IsSleeping(true)
	, m_fSleepThreshold(0.05f)
	, m_UseGravity(true)
	, m_IsKinematic(false)
	, m_fMass(1.f)
	, m_fMaterialDrag(0.02f)
	, m_fMaterialAngularDrag(0.05f)
	, m_fDrag(0.01f)
	, m_fAngularDrag(0.01f)
	, m_byConstraints(0)
	, m_vLinearAcceleration(Vec3::Zero)
	, m_vAngularAcceleration(Vec3::Zero)
	, m_vLinearVelocity(Vec3::Zero)
	, m_vAngularVelocity(Vec3::Zero)
{
}

CRigidDynamic::CRigidDynamic(const CRigidDynamic& rhs)
	: Super(rhs)
	, m_IsSleeping(rhs.m_IsSleeping)
	, m_fSleepThreshold(rhs.m_fSleepThreshold)
	, m_UseGravity(rhs.m_UseGravity)
	, m_IsKinematic(rhs.m_IsKinematic)
	, m_fMass(rhs.m_fMass)
	, m_fMaterialDrag(rhs.m_fMaterialDrag)
	, m_fMaterialAngularDrag(rhs.m_fMaterialAngularDrag)
	, m_fDrag(rhs.m_fDrag)
	, m_fAngularDrag(rhs.m_fAngularDrag)
	, m_byConstraints(rhs.m_byConstraints)
	, m_vLinearAcceleration(rhs.m_vLinearAcceleration)
	, m_vAngularAcceleration(rhs.m_vAngularAcceleration)
	, m_vLinearVelocity(rhs.m_vLinearVelocity)
	, m_vAngularVelocity(rhs.m_vAngularVelocity)
{
}

HRESULT CRigidDynamic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidDynamic::Initialize(void* pArg)
{

	return S_OK;
}

void CRigidDynamic::Tick(_float fTimeDelta)	// FixedUpdate 처럼 동작하기 위해 RigidBody의 업데이트를 가장 우선 호출해야 함.
{
	//m_vPrePosition = GetTransform()->GetPosition();

	// ColliderUpdate
	if (m_pSphereCollider)	m_pSphereCollider->Tick(fTimeDelta);
	if (m_pOBBCollider)		m_pOBBCollider->Tick(fTimeDelta);

	if (m_IsSleeping)
		return;

	if (!m_UseGravity && m_fSleepThreshold > m_vLinearVelocity.Length() && m_fSleepThreshold > 180.f * XM_1DIVPI * m_vAngularVelocity.Length())
	{
		Sleep();
		return;
	}

	if (m_IsKinematic)	// Kinematic
		KinematicUpdate(fTimeDelta);
	else				// Kinetic
		KineticUpdate(fTimeDelta);
}

void CRigidDynamic::LateTick(_float fTimeDelta)
{
}

void CRigidDynamic::DebugRender()
{
	Super::DebugRender();
}

void CRigidDynamic::KineticUpdate(_float fTimeDelta)
{
	if (m_UseGravity)
		m_vLinearVelocity.y += -9.81f * fTimeDelta /**/ * 2.f;

	m_vAngularVelocity += m_vAngularAcceleration * fTimeDelta;
	m_vLinearVelocity += m_vLinearAcceleration * fTimeDelta;

	const _float fAngularResistance = m_fAngularDrag + m_fMaterialAngularDrag;
	const _float fLinearResistance = m_fDrag + m_fMaterialDrag;

	(fAngularResistance < 1.f) ? (m_vAngularVelocity = m_vAngularVelocity * (1.f - fAngularResistance)) : (m_vAngularVelocity = Vec3::Zero);
	(fLinearResistance < 1.f) ? (m_vLinearVelocity = m_vLinearVelocity * (1.f - fLinearResistance)) : (m_vLinearVelocity = Vec3::Zero);

	// Constraints Check
	if (m_byConstraints)
	{
		_int i = 0;
		while(i < 3)
			(m_byConstraints & 1 << i) ? (*(((_float*)&m_vLinearVelocity) + i++) = 0) : i++;
		while(i < 6)
			(m_byConstraints & 1 << i) ? (*(((_float*)&m_vAngularVelocity) + i++) = 0) : i++;
	}

	UpdateTransform(fTimeDelta);
}

void CRigidDynamic::KinematicUpdate(_float fTimeDelta)
{
	UpdateTransform(fTimeDelta);

	//ClearNetPower();
	ClearForce(ForceMode::VELOCITY_CHANGE);
	ClearTorque(ForceMode::VELOCITY_CHANGE);
}

void CRigidDynamic::UpdateTransform(_float fTimeDelta)
{
	CTransform* pTransform = GetTransform();

	pTransform->Rotate(m_vAngularVelocity * fTimeDelta);
	pTransform->Translate(m_vLinearVelocity * fTimeDelta);
}

void CRigidDynamic::AddForce(const Vec3& vForce, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		m_vLinearAcceleration += vForce / m_fMass;
		break;
	case ForceMode::IMPULSE:
		m_vLinearVelocity += vForce / m_fMass;
		break;
	case ForceMode::ACCELERATION:
		m_vLinearAcceleration += vForce;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vLinearVelocity += vForce;
		break;
	}

	WakeUp();
}

void CRigidDynamic::AddTorque(const Vec3& vTorque, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:								// 전제 : r == 1	 이면 T == F
		m_vAngularAcceleration += vTorque / m_fMass;	// (점 질량)관성 모멘트 I == m * pow(r, 2) // 각가속도 a == T / I
		break;
	case ForceMode::IMPULSE:
		m_vAngularVelocity += vTorque / m_fMass;		// 각운동량 L == I * w	// 위에서 I == m // 따라서 각속도 w == L / m
		break;
	case ForceMode::ACCELERATION:
		m_vAngularAcceleration += vTorque;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vAngularVelocity += vTorque;
		break;
	}

	WakeUp();
}

void CRigidDynamic::SetForceAndTorque(const Vec3& vForce, const Vec3& vTorque, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		m_vLinearAcceleration = vForce / m_fMass;
		m_vAngularAcceleration = vTorque / m_fMass;
		break;
	case ForceMode::IMPULSE:
		m_vLinearVelocity = vForce / m_fMass;
		m_vAngularVelocity = vTorque / m_fMass;
		break;
	case ForceMode::ACCELERATION:
		m_vLinearAcceleration = vForce;
		m_vAngularAcceleration = vTorque;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vLinearVelocity = vForce;
		m_vAngularVelocity = vTorque;
		break;
	}

	WakeUp();
}

void CRigidDynamic::ClearForce(ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
		break;
	case ForceMode::IMPULSE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
		break;
	case ForceMode::ACCELERATION:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
		break;
	case ForceMode::VELOCITY_CHANGE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
		break;
	}
}

void CRigidDynamic::ClearTorque(ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:										// 전제 : r == 1	 이면 T == F
		::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));	// (점 질량)관성 모멘트 I == m * pow(r, 2) // 각가속도 a == T / I
		break;
	case ForceMode::IMPULSE:
		::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));		// 각운동량 L == I * w	// 위에서 I == m // 따라서 각속도 w == L / m
		break;
	case ForceMode::ACCELERATION:
		::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));
		break;
	case ForceMode::VELOCITY_CHANGE:
		::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));
		break;
	}
}

void CRigidDynamic::ClearNetPower()
{
	::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
	::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));
	::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));
	::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
}

void CRigidDynamic::OnCollisionEnter(const COLLISION_DESC& desc)
{
	CRigidDynamic* pRigidOther = static_cast<CRigidDynamic*>(desc.pOther);

	if (!m_IsKinematic)	// Kinetic
	{
		if (!pRigidOther->IsKinematic()) // Kinetic
		{
			AddForce(desc.vResultVelocity, ForceMode::VELOCITY_CHANGE);
			//m_vLinearVelocity = desc.vResultVelocity;
		}
		else	// Kinematic
		{
		}
	}
	else	// Kinematic
	{
		if (pRigidOther->IsKinematic())	// Kinematic
		{
			/*Vec3 vCurPosition = GetTransform()->GetPosition();
			Vec3 vOtherPosition = desc.pOther->GetTransform()->GetPosition();
			Vec3 vCollisionNormal = vCurPosition - vOtherPosition;
			Vec3 vDir = vCurPosition - m_vPrePosition;
			vCollisionNormal.Normalize();
			vCollisionNormal = Vec3::Reflect(vDir, vCollisionNormal);
			
			vDir.Normalize();*/

			//GetTransform()->Translate(11.1f * desc.fTimeDelta * desc.vResultVelocity);
		}
		else	// Kinetic
		{	
		}
	}
}

void CRigidDynamic::OnCollisionStay(const COLLISION_DESC& desc)
{
	CRigidDynamic* pRigidOther = static_cast<CRigidDynamic*>(desc.pOther);

	if (!m_IsKinematic)	// Kinetic
	{
		if (!pRigidOther->IsKinematic()) // Kinetic
		{
		}
		else	// Kinematic
		{
		}
	}
	else	// Kinematic
	{
		if (pRigidOther->IsKinematic())	// Kinematic
		{
			//GetTransform()->Translate(11.1f * desc.fTimeDelta * desc.vResultVelocity);
		}
		else	// Kinetic
		{
		}
	}
}

void CRigidDynamic::OnCollisionExit(const COLLISION_DESC& desc)
{
}

void CRigidDynamic::SetLinearVelocity(const Vec3& vLinearVelocity)
{
	m_vLinearVelocity = vLinearVelocity;

	WakeUp();
}

void CRigidDynamic::SetAngularVelocity(const Vec3& vAngularVelocity)
{
	m_vAngularVelocity = vAngularVelocity;

	WakeUp();
}

CRigidDynamic* CRigidDynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidDynamic* pInstance = new CRigidDynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidDynamic::Clone(CGameObject* pGameObject, void* pArg)
{
	CRigidDynamic* pInstance = new CRigidDynamic(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRigidDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidDynamic::Free()
{
	Super::Free();
}