#pragma once

#include "Client_Defines.h"
#include "Strife_Ammo.h"

BEGIN(Client)

class CStrife_Ammo_Beam final : public CStrife_Ammo
{
	using Super = CStrife_Ammo;

private:
	CStrife_Ammo_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStrife_Ammo_Beam(const CStrife_Ammo_Beam& rhs);
	virtual ~CStrife_Ammo_Beam() = default;

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
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

public:
	virtual	void	OnCollisionEnter(CGameObject* pOther)	override;
	virtual	void	OnCollisionStay(CGameObject* pOther)	override;
	virtual	void	OnCollisionExit(CGameObject* pOther)	override;

private:
	static _float2			m_UVoffset;

	static _float			m_fFR_Default;
	static _float			m_fFR_Default_Timer;

public:

public:
	static	CStrife_Ammo_Beam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END