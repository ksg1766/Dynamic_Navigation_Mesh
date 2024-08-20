#pragma once
#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

struct Cell;
struct Obst;

class CTransform;
class CTerrain;

struct PQNode
{
	_bool operator<(const PQNode& other) const { return f < other.f; }
	_bool operator>(const PQNode& other) const { return f > other.f; }

	_float f = FLT_MAX; // f = g + h
	_float g = FLT_MAX;
	Cell* pCell = nullptr;
};

class ENGINE_DLL CNavMeshAgent final : public CComponent
{
	using Super = CComponent;
public:
	typedef struct tagNavigationDesc
	{
		Cell* pStartCell = nullptr;
		unordered_multimap<_int, Cell*>* pCellGrids = nullptr;
		unordered_multimap<_int, Obst*>* pObstGrids = nullptr;
	}NAVIGATION_DESC;

private:
	CNavMeshAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavMeshAgent(const CNavMeshAgent& rhs);
	virtual ~CNavMeshAgent() = default;

public:
	virtual HRESULT		Initialize_Prototype()		override;
	virtual HRESULT		Initialize(void* pArg)		override;
	virtual void		Tick(_float fTimeDelta)		override;
	virtual void		LateTick(_float fTimeDelta)	override;
	void				DebugRender(_bool bRenderPathCells, _bool bRenderEntries, _bool bRenderWayPoints);
	
public:
	_bool	IsIdle();
	_bool	IsMoving();
	_bool	IsOutOfWorld(const Vec3& vPosition);

	void	ForceHeight();
	_float	GetHeightOffset();

	void	Slide(const Vec3 vPrePos);
	Vec3	Move(_float fTimeDelta);

	_bool	AStar();
	_bool	FunnelAlgorithm();

	_bool	SetPath(const Vec3& vDestPos);

	void	SetLinearSpeed(const Vec3& vLinearSpeed)	{ m_vLinearSpeed = vLinearSpeed; }
	void	SetRadius(const _float fRadius)				{ m_fAgentRadius = fRadius; }

	void	SetState(_bool isMoving)					{ m_isMoving = isMoving; }
	void	SetMoveDirectly(_bool isMovingDirectly);

public:
	void	ClearWayPoints();

private:
	void	Input(_float fTimeDelta);

private:
	_bool	AdjustLocation();
	Cell*	FindCellByPosition(const Vec3& vPosition);
	Obst*	FindObstByPosition(const Vec3& vPosition);

private:
	CTransform*		m_pTransform = nullptr;
	Vec3			m_vPrePos;
	Vec3			m_vDestPos;

	_bool			m_isMoving = false;
	_bool			m_isMovingDirectly = false;
	Vec3			m_vNetMove;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	Cell*			m_pCurrentCell = nullptr;
	Cell*			m_pDestCell = nullptr;

	_float			m_fAgentRadius;

	using PATH = pair<Cell*, LINES>;
	deque<PATH>		m_dqPath;
	deque<pair<Vec3, Vec3>>	m_dqEntries;
	deque<Vec3>		m_dqWayPoints;
	void			PopPath();

	// For Debug Render
	deque<pair<Vec3, Vec3>>	m_dqExpandedVertices;
	deque<pair<Vec3, Vec3>>	m_dqOffset;

	static unordered_multimap<_int, Cell*>* m_pCellGrids;
	static unordered_multimap<_int, Obst*>* m_pObstGrids;

//#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
//#endif

public:
	static CNavMeshAgent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END