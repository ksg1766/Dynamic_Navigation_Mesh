#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CWaterShield final : public CGameObject
{
	using Super = CGameObject;

private:
	/* ������ ������ �� */
	CWaterShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CWaterShield(const CWaterShield& rhs); /* ���� ������. */
	virtual ~CWaterShield() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

public:
	void			SetScaleRatio(_float fScaleRatio) { m_fScaleRatio = fScaleRatio; }

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

private:
	_float			m_fFrameTime = 0.f;
	_float			m_fDeleteTime = 0.f;
	_float			m_fScaleRatio = 0.f;

public:
	static	CWaterShield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END