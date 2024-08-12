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
struct Obst;
struct PQNode
{
	_bool operator<(const PQNode& other) const { return f < other.f; }
	_bool operator>(const PQNode& other) const { return f > other.f; }

	_float f = FLT_MAX; // f = g + h
	_float g = FLT_MAX;
	CellData* pCell = nullptr;
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
	_bool	IsIdle();
	_bool	IsMoving();
	_bool	IsOutOfWorld();

	void	ForceHeight();
	_float	GetHeightOffset();

	void	Slide(const Vec3 vPrePos);
	Vec3	Move(_float fTimeDelta);

	_bool	AStar();
	void	FunnelAlgorithm();

	_bool	Pick(CTerrain* pTerrain, _uint screenX, _uint screenY);

private:
	void	Input(_float fTimeDelta);

private:
	CellData* FindCellByPosition(const Vec3& vPosition);
	Obst* FindObstByPosition(const Vec3& vPosition);

private:
	CTransform*		m_pTransform = nullptr;
	Vec3			m_vPrePos;
	Vec3			m_vDestPos;

	_bool			m_isMoving = false;
	Vec3			m_vNetMove;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	pair<HierarchyNode*, CellData*> m_CurrentCell = { nullptr, nullptr };
	pair<HierarchyNode*, CellData*> m_DestCell = { nullptr, nullptr };
	
	_float			m_fAgentRadius;

	using PATH = pair<CellData*, LINES>;
	deque<PATH>		m_dqPath;
	deque<pair<Vec3, Vec3>>	m_dqEntries;
	deque<Vec3>		m_dqWayPoints;
	void			PopPath();

	// For Debug Render
	deque<pair<Vec3, Vec3>>	m_dqExpandedVertices;
	deque<pair<Vec3, Vec3>>	m_dqOffset;

	vector<HierarchyNode>* m_pHierarchyNodes;
	unordered_multimap<_int, CellData*>* m_pCellGrids;
	unordered_multimap<_int, Obst*>* m_pObstGrids;

	// DebugDraw
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

public:
	static	CAgentController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

// TODO : Pick했을 때, min dist level cell 반환 : 상위 층부터 검사하고 마지막에 base level 검사
//			-> 나중에 여유가 된다면 grid에 전부 포함시키도록... 근데 지금 terrain buffer로 pick하고 있는게 문제. 바꿀게 많다.
// 모든 cell이 자기 level 알고 있으면 좀 곤란... 그냥 pair<current level, current cell>랑 pair<dest level, dest cell>로 바꾸자.
// 거쳐갈 portal은 waypoint 처럼 deque에 넣어두고 접근했다면 waypoint와 함께 pop_front하면서 exit portal 위치로 이동.
// dest level이 current level과 다르다면 바로 가까운 portals 검색 후 해당 위치로 A*. 검색된 portal은 wayportalㄴ에 추가.

END