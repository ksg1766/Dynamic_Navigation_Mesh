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
	_int x, y, z;

	_bool operator<(const iVec3& other) const { return (x == other.x) ? z < other.z : x < other.x; }
};

struct Obst;
struct CellData;
struct HierarchyNode;
struct Portal;
class CNavMeshView final : public CView
{
    using Super = CView;
private:
	enum class TRIMODE : uint8 { DEFAULT, OBSTACLE, REGION, MODE_END };

private:
	CNavMeshView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNavMeshView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

public:
	HRESULT		DynamicCreate(CGameObject* const pGameObject);
	HRESULT		DynamicCreate(const wstring& strObjectTag, const Vec3& vPickPos);
	HRESULT		UpdateObstacleTransform(CGameObject* const pGameObject);

private:
	void		ClearNeighbors(vector<CellData*>& vecCells);
	void		SetUpNeighbors(vector<CellData*>& vecCells);
	void		SetUpCells2Grids(vector<CellData*>& vecCells, OUT unordered_multimap<_int, CellData*>& umapCellGrids, const _int iGridCX = 64U, const _int iGridCZ = 64U);
	void		SetUpObsts2Grids(vector<Obst*>& vecObstacles, OUT unordered_multimap<_int, Obst*>& umapObstGrids, const _int iGridCX = 64U, const _int iGridCZ = 64U);

	HRESULT		BakeNavMesh();
	HRESULT		BakeSingleObstacleData();
	HRESULT		BakeObstacles();
	HRESULT		BakeHeightMap3D();

	HRESULT		UpdatePointList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst = nullptr);
	HRESULT		UpdateSegmentList(triangulateio& tIn, const vector<Vec3>& vecPoints, const Obst* pObst = nullptr);
	HRESULT		UpdateHoleList(triangulateio& tIn, const Obst* pObst = nullptr);
	HRESULT		UpdateRegionList(triangulateio& tIn, const Obst* pObst = nullptr);

	HRESULT		DynamicCreate(const Obst& tObst);
	HRESULT		DynamicDelete(const Obst& tObst);

	HRESULT		CreateAgent(Vec3 vSpawnPosition);
	HRESULT		CreateAgent(_int iSpawnIndex);
	HRESULT		StressTest();

private:
	void		SetPolygonHoleCenter(Obst& tObst);
	HRESULT		GetIntersectedCells(const Obst& tObst, OUT set<CellData*>& setIntersected, _bool bPop = false, _bool bDelete = false);

private:
	HRESULT		CalculateObstacleOutline(CGameObject* const pGameObject, OUT vector<Vec3>& vecOutline);
	HRESULT		CalculateObstacleOutlines(OUT vector<vector<Vec3>>& vecOutlines, CGameObject* const pGameObject = nullptr);
	HRESULT		CalculateObstacleOutlinesTopDown(OUT vector<vector<Vec3>>& vecOutlines, CGameObject* const pGameObject = nullptr);
	HRESULT		CalculateHillOutline(OUT vector<vector<Vec3>>& vecOutlines);
	void		Dfs(const iVec3& vStart, const set<iVec3>& setPoints, OUT vector<iVec3>& vecLongest);
	void		DfsTerrain(vector<vector<_int>>& vecPoints, OUT vector<vector<iVec3>>& vecOutlines);
	Vec3		CalculateNormal(const iVec3& vPrev, const iVec3& vCurrent, const iVec3& vNext);
	_bool		IsClockwise(const vector<iVec3>& vecPoints);
	vector<Vec3> ExpandOutline(const vector<iVec3>& vecOutline, _float fDistance);
	_int		IntersectSegments(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2, Vec3* pIntersection);
	vector<Vec3> ProcessIntersections(vector<Vec3>& vecExpandedOutline);

	_float		PerpendicularDistance(const Vec3& vPt, const Vec3& vLineStart, const Vec3& vLineEnd);
	void		RamerDouglasPeucker(const vector<Vec3>& vecPointList, _float fEpsilon, OUT vector<Vec3>& vecOut);

private:
	void		Input();
	_bool		Pick(_uint screenX, _uint screenY);
	CellData*	FindCellByPosition(const Vec3& vPosition);

	HRESULT		SaveNvFile();
	HRESULT		Save3DNvFile();
	HRESULT		LoadNvFile();
	HRESULT		Load3DNvFile();
	HRESULT		DeleteNvFile();
	HRESULT		RefreshNvFile();

	HRESULT		LoadMainScene();
	//HRESULT		LoadAnotherLevelData(const vector<Vec3>& vecPoints);

	HRESULT		SaveObstacleLocalOutline(const Obst* const pObst, string strName);
	HRESULT		LoadObstacleOutlineData();

private:
	void		InfoView();
	void		PointsGroup();
	void		ObstaclesGroup();
	void		CellGroup();

private:
	HRESULT		InitialSetting();
	HRESULT		Reset();

	HRESULT		SafeReleaseTriangle(triangulateio& tTriangle);

private:
	// Path Finding (A*)
	class CAgent*			m_pAgent = nullptr;

	// Cell Data
	_int					m_iStaticPointCount = 0;

	vector<Vec3>			m_vecPoints;
	vector<vector<Vec3>>	m_vecPointsMultiLevel;
	vector<const _char*>	m_strPoints;
	unordered_multimap<_float, pair<_float, _float>>	m_umapPointHeights;

	vector<_int>			m_vecSegments;

	vector<Obst*>			m_vecObstacles;
	unordered_map<CGameObject*, _short>	m_hmapObstacleIndex;
	vector<const _char*>	m_strObstacles;
	vector<Vec3>			m_vecObstaclePoints;
	vector<const _char*>	m_strObstaclePoints;

	vector<Vec3>			m_vecRegions;
	vector<const _char*>	m_strRegions;

	unordered_multimap<_int, CellData*> m_umapCellGrids;
	unordered_multimap<_int, Obst*> m_umapObstGrids;
	vector<CellData*>		m_vecCells;
	vector<vector<Portal*>> m_vecPortalCache;
	vector<const _char*>	m_strCells;

	// Polygon (stress test)
	_bool					m_bStressTest = false;
	Obst*					m_pStressObst = nullptr;
	Matrix					m_matStressOffset = Matrix::Identity;

	// triangulate
	triangulateio			m_tIn, m_tOut;
	vector<triangulateio>	m_vecIn, m_vecOut;
	_char					m_szTriswitches[3] = "pz";

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

	map<wstring, Obst>		m_mapObstaclePrefabs;

	// Legacy
	CShader*				m_pCS_TriTest = nullptr;

public:
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};
// https://www.slideshare.net/OrenKoler1/path-finding-in-hazard-terrain
// multilayer
// https://gamedev.stackexchange.com/questions/192183/how-does-hpahierarchical-pathfinding-a-really-work
// https://web.archive.org/web/20190411040123/http://aigamedev.com/open/article/clearance-based-pathfinding/
// https://webdocs.cs.ualberta.ca/%7Emmueller/ps/hpastar.pdf
// https://gamedev.stackexchange.com/questions/99152/pathfinding-in-3d-voxel-waypoints
// https://gamedev.stackexchange.com/questions/197895/how-would-i-actually-implement-a-pathfinding-in-a-3d-world
// https://repositorium.sdum.uminho.pt/bitstream/1822/18769/1/SCCG08.pdf
END