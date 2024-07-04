#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CGeyserCrack;
class CTremorCrystal : public CGameObject
{
	using Super = CGameObject;

public:
	typedef struct tagEffectDescription
	{
		_float		fLifeTime = 0.f;

	}EFFECT_DESC;

private:
	/* 원형을 생성할 때 */
	CTremorCrystal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CTremorCrystal(const CTremorCrystal& rhs); /* 복사 생성자. */
	virtual ~CTremorCrystal() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;
	virtual HRESULT RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)	override;

public:
	void			SetLifeTime(_float fLifeTime)	{ m_fLifeTime = fLifeTime; }
	void			SetLightning()					{ m_bWithLightning = true; }

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

private:
	_float			m_fLifeTime = 0.f;
	_bool			m_bWithLightning = false;

	CGeyserCrack*	m_pCrack = nullptr;

public:
	static	CTremorCrystal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END