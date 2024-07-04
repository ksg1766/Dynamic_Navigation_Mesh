#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class COrb final : public CGameObject
{
	using Super = CGameObject;

protected:
	/* ������ ������ �� */
	COrb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	COrb(const COrb& rhs); /* ���� ������. */
	virtual ~COrb() = default;

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

	void			Fire();

private:
	_float			m_fFrameTime = 0.f;

	_bool			m_bCharged = false;
	_bool			m_bLaunched = false;

	_bool			m_bSpawned = false;
	_bool			m_bSpark = false;

	CModel*			m_pWispModel = nullptr;
	CModel*			m_pSparkModels[4] = {};

	CGameObject*	m_pSphereSwirl = nullptr;

public:
	static	COrb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END