#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"

BEGIN(Engine)

class CGameObject;
class CMonoBehaviour;

END

BEGIN(Client)

class CP_Strife_State_Idle : public CState
{
	enum Trans { RUN, AIM, JUMP, DASH, IMPACT, TRANS_END };
	enum Anims { IDLE, ANIMS_END };
	
	using Super = CState;

private:
	CP_Strife_State_Idle();
	CP_Strife_State_Idle(const CP_Strife_State_Idle& rhs) = delete;
	virtual ~CP_Strife_State_Idle() = default;
	
public:
	virtual HRESULT			Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;

	virtual const wstring&	Transition()						override;

public:
	static	CP_Strife_State_Idle* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END