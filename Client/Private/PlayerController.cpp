#include "stdafx.h"
#include "PlayerController.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Particle.h"
#include "ParticleController.h"

constexpr auto EPSILON = 0.001f;

CPlayerController::CPlayerController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(6.3f, 6.3f, 6.3f))
	, m_vMaxLinearSpeed(Vec3(20.f, 20.f, 20.f))
	, m_vAngularSpeed(Vec3(0.f, 360.f, 0.f))
	, m_vMaxAngularSpeed(Vec3(0.f, 540.f, 0.f))
{
}

CPlayerController::CPlayerController(const CPlayerController& rhs)
	:Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_vMaxLinearSpeed(rhs.m_vMaxLinearSpeed)
	, m_vAngularSpeed(rhs.m_vAngularSpeed)
	, m_vMaxAngularSpeed(rhs.m_vMaxAngularSpeed)
{
}

HRESULT CPlayerController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerController::Initialize(void* pArg)
{
	m_pTransform = GetTransform();
	m_pRigidBody = static_cast<CRigidDynamic*>(m_pGameObject->GetRigidBody());

	m_pRigidBody->FreezeRotation(Axis::X);
	m_pRigidBody->FreezeRotation(Axis::Z);

	m_pRigidBody->UseGravity(false);
	m_pRigidBody->IsKinematic(true);
	m_pRigidBody->SetMass(1.f);

	m_pRigidBody->GetSphereCollider()->GetBoundingSphere().Radius = GetTransform()->GetLocalScale().Length() / 1.8f;
	m_pRigidBody->GetOBBCollider()->GetBoundingBox().Extents = _float3(0.4f * GetTransform()->GetLocalScale().x, 0.9f * GetTransform()->GetLocalScale().y, 0.4f * GetTransform()->GetLocalScale().z);

	m_pNavMeshAgent = m_pGameObject->GetNavMeshAgent();

	return S_OK;
}

void CPlayerController::Tick(const _float& fTimeDelta)
{
	if (m_vNetMove.Length() > EPSILON)
		Move(fTimeDelta);
	else if (m_vNetTrans.Length() > EPSILON)
		Translate(fTimeDelta);

	if (!m_pNavMeshAgent->Walkable(m_pTransform->GetPosition()))
	{
		m_pTransform->SetPosition(m_vPrePos);
		//슬라이딩 벡터 = 이동 벡터 - (이동 벡터 · 충돌 표면의 노멀 벡터) * 충돌 표면의 노멀 벡터
		/*Vec3 vDir = m_pTransform->GetPosition() - m_vPrePos;
		Vec3 vPassedLine = m_pNavMeshAgent->GetPassedEdgeNormal(m_pTransform->GetPosition());
		
		m_pTransform->SetPosition(m_vPrePos + vDir - vDir.Dot(vPassedLine) * vPassedLine);*/
	}
}

void CPlayerController::LateTick(const _float& fTimeDelta)
{
	m_vPrePos = m_pTransform->GetPosition();
}

void CPlayerController::DebugRender()
{
}

_bool CPlayerController::IsIdle()
{	// 루프 끝나면 Idle로 돌아오도록 해야함.
	// 쓸일 없는 함수여야 함.
	if(!IsRun() && !IsAim() && !IsJump() && !IsDash())
		return true;
}

_bool CPlayerController::IsRun()
{
	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W) || KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A) ||
		KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S) || KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D))
		return true;

	return false;
}

_bool CPlayerController::IsAim()
{
	const POINT& p = CGameInstance::GetInstance()->GetMousePos();
	if (p.x > 1440 || p.x < 0 || p.y > 810 || p.y < 0)
		return false;

	if (MOUSE_DOWN(MOUSEKEYSTATE::DIM_LB) || MOUSE_PRESSING(MOUSEKEYSTATE::DIM_LB) ||
		MOUSE_DOWN(MOUSEKEYSTATE::DIM_RB) || MOUSE_PRESSING(MOUSEKEYSTATE::DIM_RB))
		return true;

	return false;
}

_bool CPlayerController::IsJump()
{
	if (KEY_DOWN(KEY::SPACE))
		return true;

	return false;
}

_bool CPlayerController::IsDash()
{
	if (KEY_DOWN(KEY::SHIFT))
		return true;

	return false;
}

void CPlayerController::Move(const _float& fTimeDelta)
{
	m_vNetMove.Normalize();
	Vec3 vSpeed = fTimeDelta * m_vLinearSpeed * m_vNetMove;
	m_pTransform->Translate(vSpeed);

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

void CPlayerController::Translate(const _float& fTimeDelta)
{
	m_vNetTrans.Normalize();
	Vec3 vSpeed = fTimeDelta * m_vLinearSpeed * m_vNetTrans;
	m_pTransform->Translate(vSpeed);

	m_vNetTrans = Vec3::Zero;
}

void CPlayerController::Look(const Vec3& vPoint, const _float& fTimeDelta)
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

		m_pTransform->RotateYAxisFixed(0.2f * vRotateAmount);
	}
}

void CPlayerController::OnCollisionEnter(CGameObject* pOther)
{
}

void CPlayerController::OnCollisionStay(CGameObject* pOther)
{
}

void CPlayerController::OnCollisionExit(CGameObject* pOther)
{
}

void CPlayerController::Jump()
{
	m_pRigidBody->IsKinematic(false);
	m_pRigidBody->UseGravity(true);
	m_pRigidBody->ClearForce(ForceMode::FORCE);
	m_pRigidBody->ClearForce(ForceMode::IMPULSE);
	//m_pRigidBody->AddForce(18.f * Vec3::UnitY, ForceMode::IMPULSE);
	m_pRigidBody->AddForce(15.f * Vec3::UnitY, ForceMode::IMPULSE);
}

void CPlayerController::Land()
{
	m_pRigidBody->ClearForce(ForceMode::ACCELERATION);
	m_pRigidBody->ClearForce(ForceMode::VELOCITY_CHANGE);
	m_pRigidBody->IsKinematic(true);
	m_pRigidBody->UseGravity(false);
}

void CPlayerController::Dash(const Vec3& vDir)
{
	m_pRigidBody->ClearForce(ForceMode::FORCE);
	m_pRigidBody->ClearForce(ForceMode::IMPULSE);
	m_pRigidBody->IsKinematic(false);
	m_pRigidBody->UseGravity(false);
	m_pRigidBody->AddForce(25.f * m_pTransform->GetForward(), ForceMode::IMPULSE);
}

void CPlayerController::DashEnd()
{
	m_pRigidBody->ClearForce(ForceMode::FORCE);
	m_pRigidBody->ClearForce(ForceMode::IMPULSE);
	m_pRigidBody->IsKinematic(true);
}

void CPlayerController::Fire(CStrife_Ammo::AmmoType eAmmoType)
{
	CGameObject* pAmmo = nullptr;
	CStrife_Ammo::AMMOPROPS tProps;

	Vec3 vFireOffset;
	Quaternion qRot = m_pTransform->GetRotationQuaternion();

	switch (eAmmoType)
	{
	case CStrife_Ammo::AmmoType::DEFAULT:
	{
		tProps = { eAmmoType, 7, 0, /*10*/20, 70.f * m_pTransform->GetForward(), false, 5.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Default", LAYERTAG::EQUIPMENT, &tProps);
		vFireOffset = m_pTransform->GetPosition() + 2.2f * m_pTransform->GetForward() - (m_bFireLR * 0.35f - 0.175f) * m_pTransform->GetRight() + 1.7f * m_pTransform->GetUp();
		m_bFireLR = !m_bFireLR;

		pAmmo->GetTransform()->Rotate(qRot);
		pAmmo->GetTransform()->SetPosition(vFireOffset);

		m_bFireLR ? m_pGameInstance->PlaySoundFile(TEXT("char_strife_gunfire_01.ogg"), CHANNELID::CHANNEL_GUNFIRE0, 0.3f)
			: m_pGameInstance->PlaySoundFile(TEXT("char_strife_gunfire_02.ogg"), CHANNELID::CHANNEL_GUNFIRE0, 0.3f);
	}
	break;
	case CStrife_Ammo::AmmoType::BEAM:
	{
		tProps = { eAmmoType, 4, 0, 5, m_pTransform->GetForward(), false, 0.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Beam", LAYERTAG::EQUIPMENT, &tProps);
		vFireOffset = m_pTransform->GetPosition() + 11.f * m_pTransform->GetForward() + 1.7f * m_pTransform->GetUp();

		pAmmo->GetTransform()->Rotate(qRot);
		pAmmo->GetTransform()->SetPosition(vFireOffset);
	}
	break;
	case CStrife_Ammo::AmmoType::NATURE:
	{
		for (_int i = 0; i < 17; ++i)
		{
			Vec3 vVelocity = m_pTransform->GetForward() + tanf((8 - i) * XMConvertToRadians(3.75f)) * m_pTransform->GetRight();
			vVelocity.y = 0.f;
			vVelocity.Normalize();
			CStrife_Ammo::AMMOPROPS tProps{ eAmmoType, 4, 0, 15, 20.f * vVelocity, false, 3.f };
			Quaternion qRotResult = qRot * Quaternion::CreateFromAxisAngle(Vec3::UnitY, (8 - i) * XMConvertToRadians(3.75f));
			pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Nature", LAYERTAG::EQUIPMENT, &tProps);
			vFireOffset = m_pTransform->GetPosition() + 2.2f * m_pTransform->GetForward() + 1.7f * m_pTransform->GetUp();

			pAmmo->GetTransform()->Rotate(qRotResult);
			pAmmo->GetTransform()->SetPosition(vFireOffset + 0.35f * pAmmo->GetTransform()->GetUp()); //RUL 방향 회전 돼서 다름.

			m_pGameInstance->PlaySoundFile(TEXT("char_strife_nature_shot_01.ogg"), CHANNELID::CHANNEL_GUNFIRE0, 0.3f);
			m_pGameInstance->PlaySoundFile(TEXT("char_strife_nature_projectile_01.ogg"), CHANNELID::CHANNEL_GUNFIRE1, 0.3f);
		}
	}
	break;

	/*case CStrife_Ammo::AmmoType::STATIC:
		tProps = { eAmmoType, 4, 0, 10, m_pTransform->GetForward(), false, 0.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Static", LAYERTAG::EQUIPMENT, &tProps);
		vFireOffset = m_pTransform->GetPosition() + 2.f * m_pTransform->GetForward() + 1.7f * m_pTransform->GetUp();
		break;

	*case CStrife_Ammo::AmmoType::CHARGE:
		CStrife_Ammo::AMMOPROPS tProps{ eAmmoType, 4, 0, 1, 10.f * m_pTransform->GetForward(), false, 5.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Charge", LAYERTAG::EQUIPMENT, &tProps);
		break;

	case CStrife_Ammo::AmmoType::GRAVITY:
		CStrife_Ammo::AMMOPROPS tProps{ eAmmoType, 4, 0, 1, 10.f * m_pTransform->GetForward(), false, 5.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Gravity", LAYERTAG::EQUIPMENT, &tProps);
		break;

	case CStrife_Ammo::AmmoType::LAVA:
		CStrife_Ammo::AMMOPROPS tProps{ eAmmoType, 4, 0, 1, 10.f * m_pTransform->GetForward(), false, 5.f };
		pAmmo = m_pGameInstance->CreateObject(L"Prototype_GameObject_Strife_Ammo_Lava", LAYERTAG::EQUIPMENT, &tProps);
		break;*/
	}
}

void CPlayerController::Hit()
{
	CParticleController::PARTICLE_DESC tParticleDesc;
	tParticleDesc.eType = CParticleController::ParticleType::EXPLODE;
	tParticleDesc.fScaleMax = 0.15f;
	tParticleDesc.fScaleMin = 0.1f;
	tParticleDesc.vSpeedMax = Vec3(1.8f, 2.5f, 1.8f);
	tParticleDesc.vSpeedMin = Vec3(-1.8f, 1.8f, -1.8f);
	tParticleDesc.fLifeTimeMax = 1.2f;
	tParticleDesc.fLifeTimeMin = 0.8f;
	tParticleDesc.vCenter = m_pTransform->GetPosition();
	tParticleDesc.iPass = 1;

	for (_int i = 0; i < 5; ++i)
		m_pGameInstance->CreateObject(TEXT("Prototype_GameObject_Particle"), LAYERTAG::IGNORECOLLISION, &tParticleDesc);

	m_iHitEffectCount = 7;
	m_pGameObject->GetShader()->SetPassIndex(7);
}

_bool CPlayerController::Pick(_uint screenX, _uint screenY, Vec3& pickPos, _float& distance)
{
	Matrix matBoard(1.f, 0, 0, 0,
					0, 1.f, 0, 0,
					0, 0, 1.f, 0,
					0, m_pTransform->GetPosition().y, 0, 1.f);
	return static_cast<CTerrain*>(m_pGameObject->GetFixedComponent(ComponentType::Terrain))->Pick(screenX, screenY, pickPos, distance, matBoard);
}

void CPlayerController::Input(const _float& fTimeDelta)
{
	if (KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::K))
		m_pRigidBody->IsKinematic(!m_pRigidBody->IsKinematic());
	if (KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::U))
		m_pRigidBody->UseGravity(!m_pRigidBody->UseGravity());
}

void CPlayerController::LimitAllAxisVelocity()
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

CPlayerController* CPlayerController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerController* pInstance = new CPlayerController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPlayerController::Clone(CGameObject* pGameObject, void* pArg)
{
	CPlayerController* pInstance = new CPlayerController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerController::Free()
{
	Super::Free();
}
