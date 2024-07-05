#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"

BEGIN(Client)

class CFlyingCameraController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CFlyingCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlyingCameraController(const CFlyingCameraController& rhs);
	virtual ~CFlyingCameraController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

private:
	void	Input(_float fTimeDelta);

private:
	CTransform*		m_pTransform;

	_float			m_fLinearSpeed;
	Vec3			m_vAngularSpeed;

public:
	static	CFlyingCameraController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END