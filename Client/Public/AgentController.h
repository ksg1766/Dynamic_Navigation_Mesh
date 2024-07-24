#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "CellData.h"

BEGIN(Engine)

class CTransform;
class CTerrain;

END

BEGIN(Client)

struct CellData;

struct PQNode
{
	_bool operator<(const PQNode& other) const { return f < other.f; }
	_bool operator>(const PQNode& other) const { return f > other.f; }

	_float f = FLT_MAX; // f = g + h
	_float g = FLT_MAX;
	CellData* pCell = nullptr;
	LINES ePortal = LINE_END;
};

class CAgentController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAgentController(const CAgentController& rhs);
	virtual ~CAgentController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
	void	SetCells(vector<CellData*>* pvecCells) { m_pCells = pvecCells; }	// temp

public:
	_bool	IsIdle();
	_bool	IsMoving();

	void	ForceHeight();
	_float	GetHeightOffset();
	_bool	CanMove(Vec3 vPoint);
	_bool	AStar();
	_bool	SSF();

	_bool	Pick(CTerrain* pTerrain, _uint screenX, _uint screenY);

private:
	void	Input(_float fTimeDelta);
	void	Move(_float fTimeDelta);

private:
	CellData* FindCellByPosition(const Vec3& vPosition);

private:
	CTransform*		m_pTransform = nullptr;
	Vec3			m_vPrePos;
	Vec3			m_vDestPos;

	_bool			m_isMoving = false;
	Vec3			m_vNetMove;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	CellData* m_pCurrentCell = nullptr;
	CellData* m_pDestCell = nullptr;
	
	vector<CellData*> m_vecPath;
	vector<Vec3>	m_vecWayPoints;

	vector<CellData*>* m_pCells;
	//static multimap<pair<_int, _int>, struct CellData*>* m_pCells;

	// DebugDraw
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

public:
	static	CAgentController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END