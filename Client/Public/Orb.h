#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class COrb final : public CGameObject
{
	using Super = CGameObject;

protected:
	/* 원형을 생성할 때 */
	COrb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	COrb(const COrb& rhs); /* 복사 생성자. */
	virtual ~COrb() = default;

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

	void			Fire();

private:
	_float			m_fFrameTime = 0.f;

	_bool			m_bCharged = false;
	_bool			m_bLaunched = false;

	_bool			m_bSpawned = false;
	_bool			m_bSpark = false;

	CModel*			m_pWispModel = nullptr;
	CModel*			m_pSparkModels[4] = {};

	CGameObject*	m_pSphereSwirl = nullptr;

public:
	static	COrb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END