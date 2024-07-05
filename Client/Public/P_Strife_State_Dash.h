#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CP_Strife_State_Dash : public CState
{
	enum Trans { IDLE, RUN, AIM, JUMP, IMPACT, TRANS_END };
	enum Anims { DASH, DASH_END, DASH_BACK, DASH_BACK_END, ANIMS_END };

	using Super = CState;

private:
	CP_Strife_State_Dash();
	CP_Strife_State_Dash(const CP_Strife_State_Dash& rhs) = delete;
	virtual ~CP_Strife_State_Dash() = default;
	
public:
	virtual HRESULT			Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;
	virtual const wstring&	Transition()						override;

private:
	void	Input(const _float& fTimeDelta);

private:
	_bool	m_bSoundOn = false;

public:
	static	CP_Strife_State_Dash* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END