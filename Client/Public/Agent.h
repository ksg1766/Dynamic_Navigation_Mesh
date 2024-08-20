#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTerrain;
struct Cell;
struct Obst;

END

BEGIN(Client)

class CAgentController;
class CAgent final : public CGameObject
{
	using Super = CGameObject;
private:
	CAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAgent(const CAgent& rhs);
	virtual ~CAgent() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;
	virtual HRESULT Render()					override;
	virtual HRESULT AddRenderGroup()			override;

public:
	_bool			Pick(CTerrain* pTerrain, _uint screenX, _uint screenY);
	CAgentController* GetController() {	return m_pController; };

private:
	HRESULT			Ready_FixedComponents(void* pArg);
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources();

private:
	CAgentController* m_pController = nullptr;

public:
	static	CAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END