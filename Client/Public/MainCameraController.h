#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"

BEGIN(Client)

class CMainCameraController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	enum class CameraMode { Default = 0, End };

private:
	CMainCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainCameraController(const CMainCameraController& rhs);
	virtual ~CMainCameraController() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()						override;

public:
	void	SetTarget(CTransform* pTargetTransform)	{ m_pTargetTransform = pTargetTransform; }

	void	SetCameraMode(CameraMode eCameraMode)	{ m_eCameraMode = eCameraMode; }

private:
	void	Input(_float fTimeDelta);
	void	Trace(_float fTimeDelta);

private:
	CTransform*		m_pTransform = nullptr;
	CTransform*		m_pTargetTransform = nullptr;

	Vec3			m_vOffset = Vec3(0.f, 13.f, -15.f);
	Vec3			m_vBaseOffset = Vec3(0.f, 13.f, -15.f);
	Vec3			m_vOffsetDagon = Vec3(14.f, 17.f, -14.f);
	Vec3			m_vOffsetGodRay = Vec3(-12.5f, -5.f, -17.f);
	Vec3			m_vOffsetGodRayEnd = Vec3(0.f, 4.f, -25.5f);
	_float			m_fTimer = 0.f;

	CameraMode		m_eCameraMode = CameraMode::End;

public:
	static	CMainCameraController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END