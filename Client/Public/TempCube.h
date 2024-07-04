#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CTempCube final : public CGameObject
{
	using Super = CGameObject;
private:
	CTempCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTempCube(const CTempCube& rhs);
	virtual ~CTempCube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(const _float& fTimeDelta);
	virtual void	LateTick(const _float& fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	static	CTempCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END