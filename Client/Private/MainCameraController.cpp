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

	map<LAYERTAG, CLayer*>& mapLayers = m_pGameInstance->GetCurrentLevelLayers();
	auto iter = mapLayers.find(LAYERTAG::PLAYER);
	if (iter != mapLayers.end())
		m_pTargetTransform = iter->second->GetGameObjects().front()->GetTransform();

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

void CMainCameraController::Tick(_float fTimeDelta)
{
	switch (m_eCameraMode)
	{
	case CAMERAMODE::WORLD:
		break;
	case CAMERAMODE::THIRD:
		Trace(fTimeDelta);
		break;
	}
}

void CMainCameraController::LateTick(_float fTimeDelta)
{
}

void CMainCameraController::DebugRender()
{
}

void CMainCameraController::Input(_float fTimeDelta)
{

}

void CMainCameraController::Trace(_float fTimeDelta)
{
	Vec3 vTargetPos = m_pTargetTransform->GetPosition();
	Vec3 vTargetBackward = -m_pTargetTransform->GetForward();
	vTargetBackward.Normalize();

	m_pTransform->SetPosition(vTargetPos + m_fDistance * vTargetBackward);
	
	_float fRight = m_pTransform->GetRight().Length();
	_float fUp = m_pTransform->GetUp().Length();
	_float fLook = m_pTransform->GetForward().Length();

	Vec3 vLook = vTargetPos - m_pTransform->GetPosition();
	vLook.Normalize();
	vLook *= fLook;
	m_pTransform->SetForward(vLook);

	Vec3 vRight = Vec3::UnitY.Cross(vLook);
	vRight.Normalize();
	vRight *= fRight;
	m_pTransform->SetRight(vRight);

	Vec3 vUp = vLook.Cross(vRight);
	vUp.Normalize();
	vUp *= fUp;
	m_pTransform->SetUp(vUp);

	m_pTransform->Translate(m_vOffset);
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
