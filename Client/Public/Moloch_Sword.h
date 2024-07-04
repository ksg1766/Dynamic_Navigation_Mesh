#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CFire;
class CSwordTrail;
class CMoloch_Sword final : public CGameObject
{
	using Super = CGameObject;
private:
	/* ������ ������ �� */
	CMoloch_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CMoloch_Sword(const CMoloch_Sword& rhs);
	virtual ~CMoloch_Sword() = default;

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
	HRESULT			Bind_FireResources();

	vector<CFire*>	m_vecFires;
	CSwordTrail*	m_pFire[10];

	Matrix			m_matOffsetTop;
	Matrix			m_matOffsetBottom;

public:
	static	CMoloch_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END