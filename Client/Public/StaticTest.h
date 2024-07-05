#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CStaticTest final : public CGameObject
{
	using Super = CGameObject;
private:
	CStaticTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticTest(const CStaticTest& rhs);
	virtual ~CStaticTest() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual HRESULT Render()							override;

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	static	CStaticTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END