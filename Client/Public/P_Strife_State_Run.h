#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CP_Strife_State_Run : public CState
{
	enum Trans { IDLE, AIM, JUMP, DASH, IMPACT, TRANS_END };
	enum Anims { RUN, RUN_B, ANIMS_END };

	using Super = CState;

private:
	CP_Strife_State_Run();
	CP_Strife_State_Run(const CP_Strife_State_Run& rhs) = delete;
	virtual ~CP_Strife_State_Run() = default;
	
public:
	virtual HRESULT	Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;
	virtual const wstring&	Transition()						override;

private:
	void	Input(const _float& fTimeDelta);

private:
	_float			m_fDefault = 0.63f;
	_float			m_fDefault_Timer = 0.f;

public:
	static	CP_Strife_State_Run* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END