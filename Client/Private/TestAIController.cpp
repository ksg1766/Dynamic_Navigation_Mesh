#include "stdafx.h"
#include "TestAIController.h"
#include "GameInstance.h"
#include "GameObject.h"

CTestAIController::CTestAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

CTestAIController::CTestAIController(const CTestAIController& rhs)
	:Super(rhs)
{
}

HRESULT CTestAIController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestAIController::Initialize(void* pArg)
{
	m_pRigidBody = static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody());
	m_vLinearSpeed = Vec3(100.f, 100.f, 100.f);
	m_vMaxLinearSpeed = Vec3(200.f, 200.f, 200.f);

	m_vAngularSpeed = Vec3(90.f, 90.f, 90.f);
	m_vMaxAngularSpeed = Vec3(180.f, 180.f, 180.f);

	m_pRigidBody->FreezeRotation(Axis::X);
	m_pRigidBody->FreezeRotation(Axis::Z);

	m_pRigidBody->UseGravity(false);
	m_pRigidBody->IsKinematic(false);
	m_pRigidBody->SetMass(0.5f);

	return S_OK;
}

void CTestAIController::Tick(const _float& fTimeDelta)
{
	AutoMove(fTimeDelta);
}

void CTestAIController::LateTick(const _float& fTimeDelta)
{
}

void CTestAIController::DebugRender()
{
}

void CTestAIController::AutoMove(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/*if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W))
		m_pRigidBody->AddForce(Vec3(0.f, 0.f, m_vLinearSpeed.z), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A))
		m_pRigidBody->AddForce(Vec3(-m_vLinearSpeed.x, 0.f, 0.f), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S))
		m_pRigidBody->AddForce(Vec3(0.f, 0.f, -m_vLinearSpeed.z), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D))
		m_pRigidBody->AddForce(Vec3(m_vLinearSpeed.x, 0.f, 0.f), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::Q) || KEY_DOWN(KEY::Q))
		m_pRigidBody->AddTorque(Vec3(0.f, -m_vAngularSpeed.y, 0.f), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::E) || KEY_DOWN(KEY::E))
		m_pRigidBody->AddTorque(Vec3(0.f, m_vAngularSpeed.y, 0.f), ForceMode::VELOCITY_CHANGE);

	if (KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::K))
		m_pRigidBody->IsKinematic(!m_pRigidBody->IsKinematic());
	if (KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::U))
		m_pRigidBody->UseGravity(!m_pRigidBody->UseGravity());*/

	m_pRigidBody->AddTorque(Vec3(0.f, m_vAngularSpeed.y, 0.f), ForceMode::VELOCITY_CHANGE);

	LimitAllAxisVelocity();
}

CTestAIController* CTestAIController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestAIController* pInstance = new CTestAIController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestAIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestAIController::LimitAllAxisVelocity()
{
	Vec3 pCurLinearVelocity = m_pRigidBody->GetLinearVelocity();
	Vec3 pCurAngularVelocity = m_pRigidBody->GetAngularVelocity();

	if (pCurLinearVelocity.x > m_vMaxLinearSpeed.x)
		m_pRigidBody->SetLinearAxisVelocity(Axis::X, m_vMaxLinearSpeed.x);
	if (pCurLinearVelocity.x < -m_vMaxLinearSpeed.x)
		m_pRigidBody->SetLinearAxisVelocity(Axis::X, -m_vMaxLinearSpeed.x);

	if (pCurLinearVelocity.y > m_vMaxLinearSpeed.y)
		m_pRigidBody->SetLinearAxisVelocity(Axis::Y, m_vMaxLinearSpeed.y);
	if (pCurLinearVelocity.y < -m_vMaxLinearSpeed.y)
		m_pRigidBody->SetLinearAxisVelocity(Axis::Y, -m_vMaxLinearSpeed.y);

	if (pCurLinearVelocity.z > m_vMaxLinearSpeed.z)
		m_pRigidBody->SetLinearAxisVelocity(Axis::Z, m_vMaxLinearSpeed.z);
	if (pCurLinearVelocity.z < -m_vMaxLinearSpeed.z)
		m_pRigidBody->SetLinearAxisVelocity(Axis::Z, -m_vMaxLinearSpeed.z);

	if (pCurAngularVelocity.x > m_vMaxAngularSpeed.x)
		m_pRigidBody->SetAngularAxisVelocity(Axis::X, m_vMaxAngularSpeed.x);
	if (pCurAngularVelocity.x < -m_vMaxAngularSpeed.x)
		m_pRigidBody->SetAngularAxisVelocity(Axis::X, -m_vMaxAngularSpeed.x);

	if (pCurAngularVelocity.y > m_vMaxAngularSpeed.y)
		m_pRigidBody->SetAngularAxisVelocity(Axis::Y, m_vMaxAngularSpeed.y);
	if (pCurAngularVelocity.y < -m_vMaxAngularSpeed.y)
		m_pRigidBody->SetAngularAxisVelocity(Axis::Y, -m_vMaxAngularSpeed.y);

	if (pCurAngularVelocity.z > m_vMaxAngularSpeed.z)
		m_pRigidBody->SetAngularAxisVelocity(Axis::Z, m_vMaxAngularSpeed.z);
	if (pCurAngularVelocity.z < -m_vMaxAngularSpeed.z)
		m_pRigidBody->SetAngularAxisVelocity(Axis::Z, -m_vMaxAngularSpeed.z);
}

CComponent* CTestAIController::Clone(CGameObject* pGameObject, void* pArg)
{
	CTestAIController* pInstance = new CTestAIController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTestAIController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestAIController::Free()
{
	Super::Free();
}
