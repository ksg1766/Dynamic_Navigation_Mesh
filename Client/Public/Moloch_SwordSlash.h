#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CMoloch_SwordSlash final : public CGameObject
{
	using Super = CGameObject;

public:
	typedef struct tagEffectDescription
	{
		_float	fLifeTime = 0.2f;
		_bool	bCW = false;

	}EFFECT_DESC;

private:
	/* ������ ������ �� */
	CMoloch_SwordSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CMoloch_SwordSlash(const CMoloch_SwordSlash& rhs); /* ���� ������. */
	virtual ~CMoloch_SwordSlash() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

private:
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

private:
	CTexture*		m_pNoiseTexture = nullptr;
	CTexture*		m_pMaskTexture = nullptr;

	_float2			m_UVoffset = _float2(0.f, 0.f);
	_float			m_fLifeTime = 0.2f;
	_bool			m_bCW = false;

public:
	static	CMoloch_SwordSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END