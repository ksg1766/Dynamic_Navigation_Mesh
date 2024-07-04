#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "State.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CP_Strife_State_Impact : public CState
{
	enum Trans { IDLE, RUN, AIM, JUMP, DASH, TRANS_END };
	enum Anims { IMPACT_FROMFRONT, IMPACT_FROMFBACK, IMPACT_FROMBACK, IMPACT_FROMRIGHT, ANIMS_END };

	using Super = CState;

private:
	CP_Strife_State_Impact();
	CP_Strife_State_Impact(const CP_Strife_State_Impact& rhs) = delete;
	virtual ~CP_Strife_State_Impact() = default;
	
public:
	virtual HRESULT	Enter(_int iIndex = 0)				override;

	virtual void			Tick(const _float& fTimeDelta)		override;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	override;

	virtual void			Exit()								override;
	virtual const wstring&	Transition()						override;

private:


private:
	CTransform*		m_pTransform = nullptr;
	CRigidDynamic*	m_pRigidBody = nullptr;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	Vec3			m_vMaxAngularSpeed;
	Vec3			m_vAngularSpeed;

public:
	static	CP_Strife_State_Impact* Create(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController);
	virtual void Free() override;
};

END