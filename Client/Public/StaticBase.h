#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CStaticBase : public CGameObject
{
	using Super = CGameObject;

protected:
	/* ������ ������ �� */
	CStaticBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CStaticBase(const CStaticBase& rhs); /* ���� ������. */
	virtual ~CStaticBase() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;
	virtual HRESULT RenderInstance()					override;
	virtual HRESULT RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)	override;

protected:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

public:
	static	CStaticBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END