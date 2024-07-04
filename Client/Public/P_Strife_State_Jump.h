#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CP_Strife_State_Jump : public CState
{
	enum Trans { IDLE, RUN, AIM, DASH, IMPACT, TRANS_END };
	enum Anims { JUMP, LAND, JUMP_DOUBLE, JUMP_LAND_HEAVY, FALL, ANIMS_END };

	using Super = CState;

private:
	CP_Strife_State_Jump();
	CP_Strife_State_Jump(const CP_Strife_State_Jump& rhs) = delete;
	virtual ~CP_Strife_State_Jump() = default;
	
public:
	virtual HRESULT			Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;
	virtual const wstring&	Transition()						override;

private:
	void	Input(const _float& fTimeDelta);

private:
	_bool			m_bDoubleJump = false;

public:
	static	CP_Strife_State_Jump* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END