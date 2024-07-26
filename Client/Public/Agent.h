#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTerrain;

END

BEGIN(Client)

struct CellData;
struct Obst;
class CAgentController;
class CAgent final : public CGameObject
{
	using Super = CGameObject;
public:
	struct AgentDesc
	{
		CellData* pStartCell = nullptr;
		vector<CellData*>* pCells = nullptr;
		unordered_multimap<_int, CellData*>* pCellGrids = nullptr;
		unordered_multimap<_int, Obst*>* pObstGrids = nullptr;
	};

private:
	/* 원형을 생성할 때 */
	CAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CAgent(const CAgent& rhs); /* 복사 생성자. */
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
	void			SetCells(vector<CellData*>* pvecCells, unordered_multimap<_int, CellData*>* pumapGrids);

private:
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts(void* pArg);
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

private:
	CAgentController* m_pController = nullptr;
	CTexture*		m_pNormalTexture = nullptr;

public:
	static	CAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END