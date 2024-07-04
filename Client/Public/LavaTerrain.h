#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CLavaTerrain : public CGameObject
{
	using Super = CGameObject;
protected:
	/* 원형을 생성할 때 */
	CLavaTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CLavaTerrain(const CLavaTerrain& rhs); /* 복사 생성자. */
	virtual ~CLavaTerrain() = default;

public:
	virtual HRESULT			Initialize_Prototype()				override;
	virtual HRESULT			Initialize(void* pArg)				override;
	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual void			LateTick(const _float& fTimeDelta)	override;
	virtual void			DebugRender()						override;
	virtual HRESULT			Render()							override;

protected:
	HRESULT Ready_FixedComponents(void* pArg);
	HRESULT Ready_Scripts(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CLavaTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLavaTerrain* Clone(void* pArg);
	virtual void Free() override;
};

END