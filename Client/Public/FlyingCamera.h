#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CFlyingCamera final : public CGameObject
{
	using Super = CGameObject;
private:
	CFlyingCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlyingCamera(const CFlyingCamera& rhs);
	virtual ~CFlyingCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

public:
	static	CFlyingCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END