#pragma once

#include "Client_Defines.h"
#include "Strife_Ammo.h"

BEGIN(Client)

class CStrife_Ammo_Static final : public CStrife_Ammo
{
	using Super = CStrife_Ammo;

private:
	CStrife_Ammo_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStrife_Ammo_Static(const CStrife_Ammo_Static& rhs);
	virtual ~CStrife_Ammo_Static() = default;

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

	void			FindTargets();
	void			Attack(CStrife_Ammo_Static* pAmmo, CGameObject* pTarget, const _float& fTimeDelta);

private:	// 몰라 그냥 여기 다 때려 넣어
	vector<CGameObject*>	m_vecTargets;

	static _float2			m_UVoffset;
	static _int				m_iLightningSwitch;

	static _float			m_fFR_Default;
	static _float			m_fFR_Default_Timer;

	_bool					m_IsChain = false;

public:

public:
	static	CStrife_Ammo_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END