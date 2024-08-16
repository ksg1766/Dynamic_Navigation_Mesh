#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CAIController;
class CAIAgent final : public CGameObject
{
	using Super = CGameObject;
private:
	CAIAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAIAgent(const CAIAgent& rhs);
	virtual ~CAIAgent() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;
	virtual void	DebugRender()				override;
	virtual HRESULT AddRenderGroup()			override;

public:
	void			AddWayPoint(const Vec3& vWayPoint);

private:
	HRESULT			Ready_FixedComponents(void* pArg);
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

private:
	CAIController*	m_pController = nullptr;

public:
	static	CAIAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END