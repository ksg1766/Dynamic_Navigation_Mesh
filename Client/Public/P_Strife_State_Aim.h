#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CP_Strife_State_Aim : public CState
{
	enum Trans { IDLE, RUN, JUMP, DASH, IMPACT, TRANS_END };
	enum Anims { AIM_IDLE, AIM_WALK, AIM_WALK_BACK, ANIMS_END };

	using Super = CState;

private:
	CP_Strife_State_Aim();
	CP_Strife_State_Aim(const CP_Strife_State_Aim& rhs) = delete;
	virtual ~CP_Strife_State_Aim() = default;
	
public:
	virtual HRESULT	Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;
	virtual const wstring&	Transition()						override;

private:
	void	Input(const _float& fTimeDelta);

private:
	_float			m_fFR_Default = 0.15f;
	_float			m_fFR_Default_Timer = 0.f;

public:
	static	CP_Strife_State_Aim* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END