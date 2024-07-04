#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CStrife_Ammo abstract : public CGameObject
{
	using Super = CGameObject;

public:
	enum class AmmoType { DEFAULT, CHARGE, STATIC, GRAVITY, NATURE, BEAM, LAVA, AMMO_END};
	typedef struct tagAmmoProperties
	{
		AmmoType	eAmmoType = AmmoType::AMMO_END;
		_int		iMaxKeyFrame = 0;
		_int		iKeyFrame = 0;
		_int		iDamage = 0;
		Vec3		vVelocity = {};
		_bool		isReinforced = false;
		_float		fLifeTime = 5.f;
	}AMMOPROPS;

protected:
	CStrife_Ammo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStrife_Ammo(const CStrife_Ammo& rhs);
	virtual ~CStrife_Ammo() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

protected:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

	_bool			LifeTime(const _float& fTimeDelta);
	void			Move(const _float& fTimeDelta);

protected:
	virtual	void	OnCollisionEnter(CGameObject* pOther)	override;
	virtual	void	OnCollisionStay(CGameObject* pOther)	override;
	virtual	void	OnCollisionExit(CGameObject* pOther)	override;

protected:
	AMMOPROPS		m_tProps;

public:
	virtual void	Free() override;
};

END