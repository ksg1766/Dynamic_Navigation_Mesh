#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;
class CShader;

END

BEGIN(Client)

struct iVec3
{
	_int x;
	_int y;
	_int z;

	bool operator<(const iVec3& other) const
	{
		if (x == other.x)
		{
			return z < other.z;
		}

		return x < other.x;
	}
};

typedef struct tagObstacle
{
	Vec3 vInnerPoint = Vec3::Zero;
	BoundingBox tAABB;

	vector<Vec3> vecPoints;
} Obst;

class CNavMeshView final : public CView
{
    using Super = CView;
	struct CellData;
private:
	enum POINTS	: uint8	{ POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES	: uint8	{ LINE_AB, LINE_BC, LINE_CA, LINE_END };

	enum class TRIMODE : uint8 { DEFAULT, OBSTACLE, REGION, MODE_END };
	enum class STRESSMODE : uint8 { SINGLERECT, MULTIRECT, STRESS_END };

private:
	CNavMeshView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNavMeshView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

private:
	void	ClearNeighbors(vector<CellData*>& vecCells);
	void	SetUpNeighbors(vector<CellData*>& vecCells);
	void	ShowNavMesh(_bool bOnOff) {	m_isNavMeshOn = bOnOff; }

	HRESULT	BakeNavMesh();
	HRESULT	BakeNavMeshLegacy();

	HRESULT	UpdatePointList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst = nullptr);
	HRESULT	UpdateSegmentList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst = nullptr);
	HRESULT	UpdateHoleList(triangulateio& tIn, const Obst* pObst = nullptr);
	HRESULT	UpdateRegionList(triangulateio& tIn, const Obst* pObst = nullptr);
	
	HRESULT StaticCreate(const Obst& tObst);

	HRESULT DynamicCreate(const Obst& tObst);
	HRESULT DynamicDelete(const Obst& tObst);

	HRESULT StressTest();

	HRESULT	SafeReleaseTriangle(triangulateio& tTriangle);

private:
	HRESULT	DebugRenderLegacy();
	HRESULT	RenderDT();
	HRESULT	RenderVD();

private:
	void	SetPolygonHoleCenter(Obst& tObst);
	HRESULT	GetIntersectedCells(const Obst& tObst, OUT set<CellData*>& setIntersected);

private:
	// Obstacle Outline 계산 및 영역 확장
	HRESULT CalculateObstacleOutline(CGameObject* const pGameObject, OUT vector<Vec3>& vecOutline);
	void	Dfs_(const iVec3& vCurrent, const set<iVec3>& setPoints, set<iVec3>& setVisited, OUT vector<iVec3>& vecPath, OUT vector<iVec3>& vecLongest);
	void	Dfs(const iVec3& vStart, const set<iVec3>& setPoints, OUT vector<iVec3>& vecLongest);
	Vec3	CalculateNormal(const iVec3& vPrev, const iVec3& vCurrent, const iVec3& vNext);
	_bool	IsClockwise(const vector<iVec3>& vecPoints);
	vector<Vec3> ExpandOutline(const vector<iVec3>& vecOutline, _float fDistance);
	_bool	IntersectSegments(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2, Vec3& vIntersection);
	vector<Vec3> ProcessIntersections(vector<Vec3>& vecExpandedOutline);

private:
	void	Input();
	_bool	Pick(_uint screenX, _uint screenY);

	HRESULT	SaveNvFile();
	HRESULT	LoadNvFile();
	HRESULT	DeleteNvFile();
	HRESULT	RefreshNvFile();

private:
	void	InfoView();
	void	PointsGroup();
	void	ObstaclesGroup();
	void	CellGroup();

private:
	HRESULT InitialSetting();
	HRESULT Reset();

private:
	// World Grid Data
	bitset<2048>			m_bitWorldGrid;	// 외곽선 데이터 따로 저장해두는거면 굳이 bitset은 필요 없음. 굳이 그 용도로는. 길찾기에는 필요할지도..!
	
	

	// binarize할때 minmax xz 함께 기록?

	// 만약 렌더타겟에  찍는다면?

	// triangulate
	triangulateio			m_tIn, m_tOut, m_tVD_out;
	_char					m_szTriswitches[3] = "pz";

	// Cell Data
	_int					m_iStaticPointCount = 0;

	vector<Vec3>			m_vecPoints;
	vector<const _char*>	m_strPoints;

	vector<Obst*>			m_vecObstacles;
	vector<const _char*>	m_strObstacles;
	vector<Vec3>			m_vecObstaclePoints;
	vector<const _char*>	m_strObstaclePoints;

	vector<Vec3>			m_vecRegions;
	vector<const _char*>	m_strRegions;
	
	vector<CellData*>		m_vecCells;
	vector<const _char*>	m_strCells;

	// Polygon (stress test)
	_bool					m_bStressTest = false;
	Obst*					m_pStressObst = nullptr;
	Matrix					m_matStressOffset = Matrix::Identity;

	// Default
	CTerrain*				m_pTerrainBuffer = nullptr;

	string					m_strCurrentTriangleMode = "Obstacle";
	TRIMODE					m_eCurrentTriangleMode = TRIMODE::OBSTACLE;

	// DebugDraw
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect*			m_pEffect = nullptr;
	ID3D11InputLayout*		m_pInputLayout = nullptr;

	vector<BoundingSphere>	m_vecPointSpheres;
	vector<BoundingSphere>	m_vecObstaclePointSpheres;
	vector<BoundingSphere>	m_vecRegionSpheres;

	// Datafile
	_int					m_file_Current = 0;
	_int					m_item_Current = 0;
	string					m_strFilePath = "StaticObstacles";
	vector<const _char*>	m_vecDataFiles;

	// Legacy
	_bool					m_isNavMeshOn = false;
	_float					m_fSlopeDegree = 0.0f;
	_float					m_fMaxClimb = 0.0f;
	const _float			m_fEpsilon = 0.001f;
	_float					m_fMinArea = 0.0f;
	BoundingBox				m_tNavMeshBoundVolume;

	wstring					m_strPickedObject;
	CGameObject*			m_pPickedObject = nullptr;
	CShader*				m_pCS_TriTest = nullptr;

public:
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

// DP
// https://m.blog.naver.com/dorergiverny/223113215510
// https://rosettacode.org/wiki/Ramer-Douglas-Peucker_line_simplification