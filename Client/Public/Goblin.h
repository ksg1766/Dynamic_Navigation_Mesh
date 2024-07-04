#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterController;
class CGoblin final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CGoblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CGoblin(const CGoblin& rhs); /* ���� ������. */
	virtual ~CGoblin() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;
	virtual HRESULT RenderInstance()					override;
	virtual HRESULT RenderShadow(const Matrix& matLightView, const Matrix& matLightProj)	override;
	virtual HRESULT	AddRenderGroup()					override;

private:
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

public:
	virtual	void	OnCollisionEnter(CGameObject* pOther)	override;
	virtual	void	OnCollisionStay(CGameObject* pOther)	override;
	virtual	void	OnCollisionExit(CGameObject* pOther)	override;

private:
	CMonsterController* m_pController = nullptr;

public:
	static	CGoblin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END