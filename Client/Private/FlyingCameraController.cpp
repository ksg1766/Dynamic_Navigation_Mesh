#include "stdafx.h"
#include "FlyingCameraController.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

CFlyingCameraController::CFlyingCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

CFlyingCameraController::CFlyingCameraController(const CFlyingCameraController& rhs)
	:Super(rhs)
{
}

HRESULT CFlyingCameraController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyingCameraController::Initialize(void* pArg)
{
	m_pTransform = static_cast<CTransform*>(m_pGameObject->GetTransform());
	m_pTransform->RotateYAxisFixed(Vec3(45.f, 0.f, 0.f));
	m_pTransform->Translate(Vec3(0.f, 200.f, -200.f));

	m_fLinearSpeed = 70.f;
	m_vAngularSpeed = Vec3(180.f, 180.f, 180.f);
	
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

void CFlyingCameraController::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);
}

void CFlyingCameraController::LateTick(_float fTimeDelta)
{
}

void CFlyingCameraController::DebugRender()
{
}

void CFlyingCameraController::Input(_float fTimeDelta)
{
	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W))
		m_pTransform->Translate(fTimeDelta * m_fLinearSpeed * m_pTransform->GetForward());

	if (KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A))
		m_pTransform->Translate(-fTimeDelta * m_fLinearSpeed * m_pTransform->GetRight());

	if (KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S))
		m_pTransform->Translate(-fTimeDelta * m_fLinearSpeed * m_pTransform->GetForward());

	if (KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D))
		m_pTransform->Translate(fTimeDelta * m_fLinearSpeed * m_pTransform->GetRight());

	if (KEY_PRESSING(KEY::Q) || KEY_DOWN(KEY::Q))
		m_pTransform->Translate(fTimeDelta * m_fLinearSpeed * m_pTransform->GetUp());

	if (KEY_PRESSING(KEY::E) || KEY_DOWN(KEY::E))
		m_pTransform->Translate(-fTimeDelta * m_fLinearSpeed * m_pTransform->GetUp());

	const POINT& p = m_pGameInstance->GetMousePos();
	if (p.x > 1440 || p.x < 0 || p.y > 810 || p.y < 0)
		return;

	_long		dwMouseMove = 0;

	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_LB))
			m_pTransform->RotateYAxisFixed(Vec3(0.f, -dwMouseMove * fTimeDelta * 4.f, 0.f));
	}

	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_LB))
			m_pTransform->RotateYAxisFixed(Vec3(-dwMouseMove * fTimeDelta * 4.f, 0.f, 0.f));
	}

	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_RB))
			m_pTransform->Translate(dwMouseMove * fTimeDelta * -m_pTransform->GetRight() * 3.f);
	}

	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_RB))
			m_pTransform->Translate(dwMouseMove * fTimeDelta * m_pTransform->GetUp() * 3.f);
	}
}

CFlyingCameraController* CFlyingCameraController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlyingCameraController* pInstance = new CFlyingCameraController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlyingCameraController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CFlyingCameraController::Clone(CGameObject* pGameObject, void* pArg)
{
	CFlyingCameraController* pInstance = new CFlyingCameraController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFlyingCameraController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlyingCameraController::Free()
{
}
