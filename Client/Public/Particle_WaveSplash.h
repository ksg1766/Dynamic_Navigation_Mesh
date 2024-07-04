#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)



END

BEGIN(Client)

class CParticle_WaveSplash final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CParticle_WaveSplash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CParticle_WaveSplash(const CParticle_WaveSplash& rhs); /* ���� ������. */
	virtual ~CParticle_WaveSplash() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

public:
	void	SetEmitDirection(const Vec3& vEmitDirection) { m_vEmitDirection = vEmitDirection; }

private:
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

private:
	CShader*	m_pShaderStream = nullptr;
	_float		m_fLifeTime = 0.f;
	_float		m_fTimeDelta = 0.f;

	CTexture*	m_pTextureEx = nullptr;

	ID3D11ShaderResourceView*	m_pRandomTexture = nullptr;

	Vec3		m_vEmitDirection;

public:
	static	CParticle_WaveSplash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END