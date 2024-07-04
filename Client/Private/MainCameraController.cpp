#include "stdafx.h"
#include "MainCameraController.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

CMainCameraController::CMainCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

CMainCameraController::CMainCameraController(const CMainCameraController& rhs)
	:Super(rhs)
{
}

HRESULT CMainCameraController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCameraController::Initialize(void* pArg)
{
	m_pTransform = m_pGameObject->GetTransform();
	
	m_vOffset = Vec3(0.f, 13.f, -15.f);

	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	auto iter = mapLayers.find(LAYERTAG::PLAYER);
	if (iter == mapLayers.end())
		return E_FAIL;

	m_pTargetTransform = iter->second->GetGameObjects().front()->GetTransform();
	
	//m_pPlayer = ;//GameManager ���� �Ŷ� ������

	/*CCamera::CAMERA_DESC tDesc;
	::ZeroMemory(&tDesc, sizeof CCamera::CAMERA_DESC);

	tDesc.vEye = Vec4(m_pTransform->GetPosition(), 1.f);
	tDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tDesc.fFovy = XMConvertToRadians(60.0f);
	tDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	tDesc.fNear = 0.1f;
	tDesc.fFar = 1000.0f;*/

	return S_OK;
}

void CMainCameraController::Tick(const _float& fTimeDelta)
{
	switch (m_eCameraMode)
	{
	case CameraMode::Default:
		//m_vOffset = m_vBaseOffset;
		//m_fTimer = 0.f;
		break;
	case CameraMode::Dagon:
		Dagon(fTimeDelta);
		break;
	case CameraMode::DagonToBase:
		DagonToBase(fTimeDelta);
		break;
	case CameraMode::GodRay:
		GodRayScene(fTimeDelta);
		break;
	case CameraMode::GodRayToBase:
		GodRayEnd(fTimeDelta);
		break;
	case CameraMode::Moloch:
		//Moloch(fTimeDelta);
		break;
	case CameraMode::MolochToBase:
		//MolochToBase(fTimeDelta);
		break;
	}

	//Input(fTimeDelta);
	
	Trace(fTimeDelta);
}

void CMainCameraController::LateTick(const _float& fTimeDelta)
{
}

void CMainCameraController::DebugRender()
{
}

void CMainCameraController::Input(const _float& fTimeDelta)
{

}

void CMainCameraController::Trace(const _float& fTimeDelta)
{
	const Vec3& vTargetPos = m_pTargetTransform->GetPosition() + Vec3::UnitY;
	//Vec3 vPos = m_pTransform->GetPosition();
	Vec3 vDist = vTargetPos - m_pTransform->GetPosition();
	//Vec3::Lerp(vPos, m_vOffset + vTargetPos, max(10.f * fTimeDelta, 1.f), vPos);

	//m_pTransform->SetPosition(vPos);
	m_pTransform->SetPosition(m_vOffset + vTargetPos);

	vDist.Normalize();
	
	Matrix& matWorld = m_pTransform->WorldMatrix();

	_float fRight = matWorld.Right().Length();
	_float fUp = matWorld.Up().Length();
	_float fLook = matWorld.Backward().Length();

	matWorld.Backward(/*fLook * */vDist);
	Vec3 vRight = Vec3::UnitY.Cross(vDist);
	vRight.Normalize();
	matWorld.Right(/*fRight * */vRight);
	Vec3 vUp = vDist.Cross(vRight);
	vUp.Normalize();
	matWorld.Up(/*fUp * */vUp);
}

void CMainCameraController::Dagon(const _float& fTimeDelta)
{
	if (Vec3::DistanceSquared(m_vOffset, m_vOffsetDagon) < 1.f)
	{
		m_fTimer = 0.f;
		m_vOffset = m_vOffsetDagon;
		m_eCameraMode = CameraMode::Default;
	}
	else
		m_vOffset = Vec3::Lerp(m_vOffset, m_vOffsetDagon, m_fTimer);
		//m_vOffset = Vec3::Lerp(m_vBaseOffset, m_vOffsetDagon, m_fTimer);

	m_fTimer += 0.003f * fTimeDelta;
}

void CMainCameraController::DagonToBase(const _float& fTimeDelta)
{
	if (Vec3::DistanceSquared(m_vOffset, m_vBaseOffset) < 1.f)
	{
		m_fTimer = 0.f;
		m_vOffset = m_vBaseOffset;
		m_eCameraMode = CameraMode::Default;
	}
	else
		m_vOffset = Vec3::Lerp(m_vOffset, m_vBaseOffset, m_fTimer);
		//m_vOffset = Vec3::Lerp(m_vBaseOffset, m_vOffsetDagon, m_fTimer);

	m_fTimer += 0.007f * fTimeDelta;
}

void CMainCameraController::Moloch(const _float& fTimeDelta)
{
}

void CMainCameraController::MolochToBase(const _float& fTimeDelta)
{
}

void CMainCameraController::GodRayScene(const _float& fTimeDelta)
{
	if (Vec3::DistanceSquared(m_vOffset, m_vOffsetGodRay) < 1.f)
	{
		m_fTimer = 0.f;
		m_vOffset = m_vOffsetGodRay;
		m_eCameraMode = CameraMode::Default;
	}
	else
		m_vOffset = Vec3::Lerp(m_vOffset, m_vOffsetGodRay, m_fTimer);
	//m_vOffset = Vec3::Lerp(m_vBaseOffset, m_vOffsetDagon, m_fTimer);

	m_fTimer += 0.005f * fTimeDelta;
}

void CMainCameraController::GodRayEnd(const _float& fTimeDelta)
{
	if (Vec3::DistanceSquared(m_vOffset, m_vOffsetGodRayEnd) < 1.f)
	{
		m_fTimer = 0.f;
		m_vOffset = m_vOffsetGodRayEnd;
		m_eCameraMode = CameraMode::Default;
	}
	else
		m_vOffset = Vec3::Lerp(m_vOffset, m_vOffsetGodRayEnd, m_fTimer);
	//m_vOffset = Vec3::Lerp(m_vBaseOffset, m_vOffsetDagon, m_fTimer);

	m_fTimer += 0.002f * fTimeDelta;
}

CMainCameraController* CMainCameraController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainCameraController* pInstance = new CMainCameraController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainCameraController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMainCameraController::Clone(CGameObject* pGameObject, void* pArg)
{
	CMainCameraController* pInstance = new CMainCameraController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMainCameraController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainCameraController::Free()
{
}
