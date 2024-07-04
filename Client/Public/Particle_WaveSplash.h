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
	/* 원형을 생성할 때 */
	CParticle_WaveSplash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CParticle_WaveSplash(const CParticle_WaveSplash& rhs); /* 복사 생성자. */
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
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */
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
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END