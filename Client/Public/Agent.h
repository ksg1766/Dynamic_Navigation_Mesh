#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CAgentController;
class CAgent final : public CGameObject
{
	using Super = CGameObject;

private:
	/* ������ ������ �� */
	CAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CAgent(const CAgent& rhs); /* ���� ������. */
	virtual ~CAgent() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;
	virtual HRESULT Render()					override;
	virtual HRESULT AddRenderGroup()			override;

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts(void* pArg);
	HRESULT			Bind_ShaderResources(); /* ���̴� ���������� �� ������. */

private:
	CAgentController* m_pController = nullptr;
	CTexture*		m_pNormalTexture = nullptr;

public:
	static	CAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END