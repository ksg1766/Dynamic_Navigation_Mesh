#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CParticle_WaveSplash;
class CParticle_Waterfall;
class CDagonWave final : public CGameObject
{
	using Super = CGameObject;
private:
	/* 원형을 생성할 때 */
	CDagonWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CDagonWave(const CDagonWave& rhs); /* 복사 생성자. */
	virtual ~CDagonWave() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	virtual	void	OnCollisionEnter(CGameObject* pOther)	override;
	virtual	void	OnCollisionStay(CGameObject* pOther)	override;
	virtual	void	OnCollisionExit(CGameObject* pOther)	override;

private:
	_float			m_fFrameTime = 0.f;
	CParticle_WaveSplash*	m_pSplash = nullptr;

public:
	static	CDagonWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END