#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTerrain;

END

BEGIN(Client)

struct CellData;
struct HierarchyNode;
struct Obst;
class CAgentController;
class CAgent final : public CGameObject
{
	using Super = CGameObject;
public:
	struct AgentDesc
	{
		CellData* pStartCell = nullptr;
		unordered_multimap<_int, CellData*>* pCellGrids = nullptr;
		unordered_multimap<_int, Obst*>* pObstGrids = nullptr;
	};

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

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts(void* pArg);
	HRESULT			Bind_ShaderResources();

private:
	CAgentController* m_pController = nullptr;
	CTexture*		m_pNormalTexture = nullptr;

public:
	static	CAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END