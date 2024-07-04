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
		// srv, tweendesc �ʿ�.
		CModel*		pModel		= nullptr;
		TWEENDESC*	pTweendesc	= nullptr;
		Matrix		matWorld;
		_float		fLifeTime	= 0.f;

	}MOTIONTRAIL_DESC;

private:
	/* ������ ������ �� */
	CStrife_MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CStrife_MotionTrail(const CStrife_MotionTrail& rhs); /* ���� ������. */
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
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

private:
	CModel*			m_pModel = nullptr;
	TWEENDESC		m_tTweendesc;
	_float			m_fLifeTime = 0.f;

public:
	static	CStrife_MotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END