#include "stdafx.h"
#include "BossController.h"
#include "MonsterStats.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "DebugDraw.h"
#include "Particle.h"
#include "ParticleController.h"

constexpr auto EPSILON = 0.001f;

CBossController::CBossController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(5.f, 5.f, 5.f))
	, m_vMaxLinearSpeed(Vec3(20.f, 20.f, 20.f))
	, m_vAngularSpeed(Vec3(0.f, 360.f, 0.f))
	, m_vMaxAngularSpeed(Vec3(0.f, 540.f, 0.f))
{
}

CBossController::CBossController(const CBossController& rhs)
	:Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_vMaxLinearSpeed(rhs.m_vMaxLinearSpeed)
	, m_vAngularSpeed(rhs.m_vAngularSpeed)
	, m_vMaxAngularSpeed(rhs.m_vMaxAngularSpeed)
	, m_IsZeroHP(rhs.m_IsZeroHP)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CBossController::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t		iLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
		return E_FAIL;
	}
#endif

	return S_OK;
}

HRESULT CBossController::Initialize(void* pArg)
{
	m_pTransform = GetTransform();
	m_pRigidBody = static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody());

	m_pRigidBody->FreezeRotation(Axis::X);
	m_pRigidBody->FreezeRotation(Axis::Z);

	m_pRigidBody->UseGravity(false);
	m_pRigidBody->IsKinematic(true);
	m_pRigidBody->SetMass(2.f);

	m_pNavMeshAgent = m_pGameObject->GetNavMeshAgent();

	return S_OK;
}

void CBossController::Tick(const _float& fTimeDelta)
{
	if (0 < m_iHitEffectCount)
	{
		--m_iHitEffectCount;
	}
	else if (0 == m_iHitEffectCount)
	{
		m_pGameObject->GetShader()->SetPassIndex(0);
		--m_iHitEffectCount;
	}

	if (m_vNetMove.Length() > EPSILON)
		Move(fTimeDelta);
	else if (m_vNetTrans.Length() > EPSILON)
		Translate(fTimeDelta);

	if (!m_pNavMeshAgent)
		return;

	if (!m_pNavMeshAgent->Walkable(m_pTransform->GetPosition()))
	{
		m_pTransform->SetPosition(m_vPrePos);
	}
}

void CBossController::LateTick(const _float& fTimeDelta)
{
	if (!m_pNavMeshAgent)
		return;

	m_vPrePos = m_pTransform->GetPosition();
}

void CBossController::DebugRender()
{
#ifdef _DEBUG
	//m_pEffect->SetWorld(XMMatrixIdentity());

	//m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	//m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//m_pEffect->Apply(m_pContext);
	//m_pContext->IASetInputLayout(m_pInputLayout);

	//m_pBatch->Begin();

	//if (TEXT("Moloch") == m_pGameObject->GetObjectTag())
	//{
	//	DX::DrawRing(m_pBatch, GetTransform()->GetPosition(), Vec3(10.f, 0.f, 0.f), Vec3(0.f, 0.f, 10.f), Colors::Red);
	//	DX::DrawRing(m_pBatch, GetTransform()->GetPosition(), Vec3(4.f, 0.f, 0.f), Vec3(0.f, 0.f, 4.f), Colors::Blue);
	//}
	//
	//m_pBatch->End();
#endif // DEBUG
}

void CBossController::GetHitMessage(_int iDamage)
{
	Hit(iDamage);
	// BT BlackBoard에 Hit 상태 전달 할 수 있도록.
}

void CBossController::Move(const _float& fTimeDelta)
{
	m_vNetMove.Normalize();

	/*Vec3 vSpeed = (m_bMax ? m_vMaxLinearSpeed : m_vLinearSpeed) * fTimeDelta * m_vNetMove;
	m_pTransform->Translate(vSpeed);*/ //TODO: 테스트 해볼 것

	Vec3 vSpeed = fTimeDelta * (m_bMax * m_vMaxLinearSpeed + (1 - m_bMax) * m_vLinearSpeed) * m_vNetMove;
	m_pTransform->Translate(vSpeed);
	m_bMax = false;

	const Vec3& vForward = m_pTransform->GetForward();
	_float fRadian = acos(vForward.Dot(m_vNetMove));

	if (fabs(fRadian) > EPSILON)
	{
		const Vec3& vLeftRight = vForward.Cross(m_vNetMove);
		//Vec3 vRotateAmount(m_vAngularSpeed * fRadian);
		Vec3 vRotateAmount(0.f, XMConvertToDegrees(fRadian), 0.f);
		if (vLeftRight.y < 0)
			vRotateAmount.y = -vRotateAmount.y;

		m_pTransform->RotateYAxisFixed(0.2f * vRotateAmount);
	}

	m_vNetMove = Vec3::Zero;
}

void CBossController::Translate(const _float& fTimeDelta)
{
	m_vNetTrans.Normalize();

	Vec3 vSpeed = fTimeDelta * (m_bMax * m_vMaxLinearSpeed + (1 - m_bMax) * m_vLinearSpeed) * m_vNetTrans;
	m_pTransform->Translate(vSpeed);
	m_bMax = false;

	m_vNetTrans = Vec3::Zero;
}

void CBossController::Dash(const Vec3& vDir)
{
	m_pRigidBody->ClearForce(ForceMode::FORCE);
	m_pRigidBody->ClearForce(ForceMode::IMPULSE);
	m_pRigidBody->IsKinematic(false);
	m_pRigidBody->UseGravity(false);
	m_pRigidBody->AddForce(105.f * m_pTransform->GetForward(), ForceMode::IMPULSE);
}

void CBossController::DashEnd()
{
	m_pRigidBody->ClearForce(ForceMode::FORCE);
	m_pRigidBody->ClearForce(ForceMode::IMPULSE);
	m_pRigidBody->IsKinematic(true);
}

void CBossController::Attack(_int iSkillIndex)
{
	/*switch (iSkillIndex)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	}*/

	/*if (TEXT("HellHound") == m_pGameObject->GetObjectTag())
	{
		
	}
	else if (TEXT("HellBrute") == m_pGameObject->GetObjectTag())
	{
		if ()
	}*/
}

void CBossController::Hit(_int iDamage)
{
	m_pStats->Damaged(iDamage);

	CParticleController::PARTICLE_DESC tParticleDesc;
	tParticleDesc.eType = CParticleController::ParticleType::FLY;
	tParticleDesc.vCenter = m_pTransform->GetPosition();
	tParticleDesc.iPass = 1;
	for (_int i = 0; i < 25; ++i)
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

	if (m_pStats->GetHP() <= 0)
	{
		m_IsZeroHP = true;
		m_pRigidBody->IsKinematic(false);
		return;
	}

	m_iHitEffectCount = 20;
	m_pGameObject->GetShader()->SetPassIndex(1);
}

void CBossController::Look(const Vec3& vPoint, const _float& fTimeDelta)
{
	Vec3 vDir = vPoint - m_pTransform->GetPosition();
	vDir.y = 0.f;
	vDir.Normalize();
	const Vec3& vForward = m_pTransform->GetForward();

	_float fRadian = acos(vForward.Dot(vDir));
	if (fabs(fRadian) > EPSILON)
	{
		const Vec3& vLeftRight = vForward.Cross(vDir);
		Vec3 vRotateAmount(0.f, XMConvertToDegrees(fRadian), 0.f);
		if (vLeftRight.y < 0)
			vRotateAmount.y = -vRotateAmount.y;

		m_pTransform->RotateYAxisFixed(fTimeDelta * vRotateAmount);
	}
}

void CBossController::OnCollisionEnter(CGameObject* pOther)
{
}

void CBossController::OnCollisionStay(CGameObject* pOther)
{
}

void CBossController::OnCollisionExit(CGameObject* pOther)
{
}

void CBossController::LimitAllAxisVelocity()
{
	Vec3 pCurLinearVelocity = m_pRigidBody->GetLinearVelocity();

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
}

CBossController* CBossController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossController* pInstance = new CBossController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBossController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBossController::Clone(CGameObject* pGameObject, void* pArg)
{
	CBossController* pInstance = new CBossController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBossController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossController::Free()
{
	Super::Free();
}
