#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CStaticBase : public CGameObject
{
	using Super = CGameObject;

protected:
	/* 원형을 생성할 때 */
	CStaticBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CStaticBase(const CStaticBase& rhs); /* 복사 생성자. */
	virtual ~CStaticBase() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;
	virtual HRESULT Render()					override;
	virtual HRESULT RenderInstance()			override;
	virtual HRESULT RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)	override;

protected:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	static	CStaticBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END