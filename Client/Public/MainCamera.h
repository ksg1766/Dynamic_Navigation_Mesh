#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMainCameraController;
class CMainCamera final : public CGameObject
{
	using Super = CGameObject;
private:
	CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;

public:
	CMainCameraController* GetController() { return m_pController; }

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

	CMainCameraController* m_pController = nullptr;

public:
	static	CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END