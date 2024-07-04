#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CLightning_Spark final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CLightning_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CLightning_Spark(const CLightning_Spark& rhs); /* ���� ������. */
	virtual ~CLightning_Spark() = default;

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

public:
	virtual	void	OnCollisionEnter(CGameObject* pOther)	override;
	virtual	void	OnCollisionStay(CGameObject* pOther)	override;
	virtual	void	OnCollisionExit(CGameObject* pOther)	override;

private:
	_float			m_fFrameTime = 0.f;
	_float			m_fEmissivePower = 1.f;

	_bool			m_bWaterSplash = false;
	_byte			m_byteFlag = 5;
	CModel*			m_pSparkModels[4] = {};

	CShader*		m_pShaderStream = nullptr;
	CShader*		m_pShaderDraw = nullptr;

public:
	static	CLightning_Spark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END