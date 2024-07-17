#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;
class CShader;

END

BEGIN(Client)

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

	enum class TRIMODE	: uint8	{ DEFAULT, OBSTACLE, REGION, MODE_END };

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

	HRESULT	SafeReleaseTriangle(triangulateio& tTriangle);

private:
	HRESULT	DebugRenderLegacy();
	HRESULT	RenderDT();
	HRESULT	RenderVD();

private:
	void	SetPolygonHoleCenter(Obst& tObst);
	void	GetIntersectedCells(const Obst& tObst, OUT set<CellData*>& setIntersected);

private:
	void	Input();
	_bool	Pick(_uint screenX, _uint screenY);

	HRESULT	SaveFile();
	HRESULT	LoadFile();
	HRESULT	DeleteFile();
	HRESULT	RefreshFile();

private:
	void	InfoView();
	void	PointsGroup();
	void	ObstaclesGroup();
	void	CellGroup();

private:
	HRESULT InitialSetting();
	HRESULT Reset();

private:
	// triangulate
	triangulateio			m_tIn, m_tOut, m_tVD_out;
	_char					m_szTriswitches[3] = "pz";

	// Cell Data
	_int					m_iStaticPointCount = 0;

	vector<Vec3>			m_vecPoints;
	vector<const _char*>	m_strPoints;

	vector<Obst>			m_vecObstacles;
	vector<const _char*>	m_strObstacles;
	vector<Vec3>			m_vecObstaclePoints;
	vector<const _char*>	m_strObstaclePoints;

	vector<Vec3>			m_vecRegions;
	vector<const _char*>	m_strRegions;
	
	vector<CellData*>		m_vecCells;
	vector<const _char*>	m_strCells;

	// Polygon (stress test)
	Obst					m_tStressObst;

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
	CShader*				m_pShader = nullptr;

public:
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END