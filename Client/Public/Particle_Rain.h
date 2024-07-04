#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)



END

BEGIN(Client)

class CParticle_Rain final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CParticle_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CParticle_Rain(const CParticle_Rain& rhs); /* ���� ������. */
	virtual ~CParticle_Rain() = default;

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
	CShader*	m_pShaderStream = nullptr;
	_float		m_fLifeTime = 0.f;
	_float		m_fTimeDelta = 0.f;

	ID3D11ShaderResourceView*	m_pRandomTexture = nullptr;

public:
	static	CParticle_Rain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END