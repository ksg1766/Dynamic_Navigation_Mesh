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
	
	//m_pPlayer = ;//GameManager 같은 거라도 만들자

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
	case CameraMode::Default:
		//m_vOffset = m_vBaseOffset;
		//m_fTimer = 0.f;
		break;
	}

	//Input(fTimeDelta);
	
	Trace(fTimeDelta);
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
