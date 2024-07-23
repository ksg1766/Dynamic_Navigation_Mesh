#pragma once

#include "Client_Defines.h"
#include "StaticBase.h"

BEGIN(Client)

class CStaticScene final : public CStaticBase
{
	using Super = CStaticBase;

protected:
	/* ������ ������ �� */
	CStaticScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CStaticScene(const CStaticScene& rhs); /* ���� ������. */
	virtual ~CStaticScene() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;
	virtual HRESULT RenderInstance()					override;

protected:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

public:
	static	CStaticScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END