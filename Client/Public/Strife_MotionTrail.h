#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CShader;

END

BEGIN(Client)

class CStrife_MotionTrail final : public CGameObject
{
	using Super = CGameObject;

public:
	typedef struct tagMotionTrailDescription
	{
		// srv, tweendesc 필요.
		CModel*		pModel		= nullptr;
		TWEENDESC*	pTweendesc	= nullptr;
		Matrix		matWorld;
		_float		fLifeTime	= 0.f;

	}MOTIONTRAIL_DESC;

private:
	/* 원형을 생성할 때 */
	CStrife_MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CStrife_MotionTrail(const CStrife_MotionTrail& rhs); /* 복사 생성자. */
	virtual ~CStrife_MotionTrail() = default;

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

private:
	CModel*			m_pModel = nullptr;
	TWEENDESC		m_tTweendesc;
	_float			m_fLifeTime = 0.f;

public:
	static	CStrife_MotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END