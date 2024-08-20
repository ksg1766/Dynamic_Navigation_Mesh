#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"

BEGIN(Client)

class CMainCameraController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	enum class CAMERAMODE : uint8 { WORLD, FIRST, THIRD, MODE_END };

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

	void	SetCameraMode(CAMERAMODE eCameraMode)	{ m_eCameraMode = eCameraMode; }

private:
	void	Input(_float fTimeDelta);
	void	Trace(_float fTimeDelta);

private:
	CTransform*		m_pTransform = nullptr;
	CTransform*		m_pTargetTransform = nullptr;

	Vec3			m_vOffset = Vec3(0.0f, 40.0f, 0.0f);
	_float			m_fDistance = 180.0f;

	CAMERAMODE		m_eCameraMode = CAMERAMODE::THIRD;

public:
	static	CMainCameraController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END