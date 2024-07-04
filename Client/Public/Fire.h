#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)



END

BEGIN(Client)

class CFire final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CFire(const CFire& rhs); /* ���� ������. */
	virtual ~CFire() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

public:
	void			SetPivotMatrix(Matrix& matPivot) { m_matPivot = matPivot; }

private:
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts(void* pArg);

private:
	CTexture*	m_pNoiseTexture = nullptr;
	CTexture*	m_pAlphaTexture = nullptr;

	_float		m_fFrameTime = 0.f;

	Matrix		m_matPivot = Matrix::Identity;

public:
	static	CFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END