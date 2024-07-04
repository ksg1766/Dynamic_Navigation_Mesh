#include "..\Public\CameraManager.h"
#include "Level.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCameraManager)

CCameraManager::CCameraManager()
{
}

HRESULT CCameraManager::Reserve_Manager(_uint iNumLevels)
{
	return S_OK;
}

void CCameraManager::Tick(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentCamera)
		return;

	m_pCurrentCamera->Tick(fTimeDelta);
}

void CCameraManager::LateTick(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentCamera)
		return;

	m_pCurrentCamera->LateTick(fTimeDelta);

	if (KEY_PRESSING_EX(KEY::CTRL) && KEY_DOWN_EX(KEY::F9))
		ChangeCamera(TEXT("FlyingCamera"));
	else if (KEY_PRESSING_EX(KEY::CTRL) && KEY_DOWN_EX(KEY::F8))
		ChangeCamera(TEXT("MainCamera"));
}

void CCameraManager::DebugRender()
{
	if (nullptr == m_pCurrentCamera)
		return;

	m_pCurrentCamera->DebugRender();
}

HRESULT CCameraManager::AddCamera(const wstring& strName, CGameObject* pCamera)
{
	CameraHash::iterator iter;
	iter = m_hashCamera.find(strName);

	if (iter == m_hashCamera.end())
	{
		m_hashCamera.emplace(strName, pCamera);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CCameraManager::DeleteCamera(const wstring& strName)
{
	CameraHash::iterator iter;
	iter = m_hashCamera.find(strName);

	if (iter == m_hashCamera.end())
		return E_FAIL;

	m_hashCamera.erase(iter);

	return E_FAIL;
}

HRESULT CCameraManager::ChangeCamera(const wstring& strName)
{
	CameraHash::iterator iter;
	iter = m_hashCamera.find(strName);

	if (iter == m_hashCamera.end())
		return E_FAIL;

	m_pCurrentCamera = iter->second;

	return S_OK;
}

void CCameraManager::UpdateReflectionMatrix(_float fWaterLevel)
{
	CTransform* pCamTransform = m_pCurrentCamera->GetTransform();
	const Vec3& vCamPosition = pCamTransform->GetPosition();
	const Vec3& vCamForward = pCamTransform->GetForward();
	const Vec3& vCamRight = pCamTransform->GetRight();

	Vec3 vTargetPos = vCamPosition + vCamForward;

	_float fReflectionCamYCoord = -vCamPosition.y + 2 * fWaterLevel;
	Vec3 vReflectionCamPos(vCamPosition.x, fReflectionCamYCoord, vCamPosition.z);

	_float fReflectionTargetYCoord = -vTargetPos.y + 2 * fWaterLevel;
	Vec3 vReflectionCamTarget(vTargetPos.x, fReflectionTargetYCoord, vTargetPos.z);

	Vec3 vForward = vReflectionCamTarget - vReflectionCamPos;
	Vec3 vReflectionCamUp = vCamRight.Cross(vForward);

	m_matReflect = XMMatrixLookAtLH(vReflectionCamPos, vReflectionCamTarget, vReflectionCamUp);
}

void CCameraManager::Free()
{
	Safe_Release(m_pCurrentCamera);
	Super::Free();
}
