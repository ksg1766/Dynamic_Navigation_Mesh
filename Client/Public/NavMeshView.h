#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;
class CShader;

END

struct tagObstacle
{
	_int start = -1;					// index of start point
	_int numberof = 0;					// number of points of this obstakle
	Vec3 center = Vec3(0.f, 0.f, 0.f);	// center of gravity
};

using Obst = tagObstacle;

BEGIN(Client)

class CNavMeshView final : public CView
{
    using Super = CView;
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
	struct	CellData;
	void	ClearNeighbors(vector<CellData*>& vecCells);
	void	SetUpNeighbors(vector<CellData*>& vecCells);
	void	ShowNavMesh(_bool bOnOff) {	m_isNavMeshOn = bOnOff; }

	HRESULT	BakeNavMesh();
	HRESULT	UpdatePointList();
	HRESULT	UpdateSegmentList();
	HRESULT	UpdateHoleList();
	HRESULT	UpdateRegionList();

	HRESULT	SafeReleaseTriangle(triangulateio& tTriangle);

private:
	HRESULT	DebugRenderLegacy();
	HRESULT	RenderDT();
	HRESULT	RenderVD();

private:
	void	SetPolygonHoleCenter(Obst& tObst);

private:
	void	Input();
	_bool	Pick(_uint screenX, _uint screenY);

	HRESULT	Save();
	HRESULT	Load();

private:
	void	InfoView();
	void	PointsGroup();
	void	ObstaclePointsGroup();
	void	CellGroup();

private:
	_bool				m_isNavMeshOn = false;
	_float				m_fSlopeDegree = 0.0f;
	_float				m_fMaxClimb = 0.0f;
	const _float		m_fEpsilon = 0.001f;
	_float				m_fMinArea = 0.0f;

	BoundingBox			m_tNavMeshBoundVolume;

	CTerrain*			m_pTerrainBuffer = nullptr;

	// DT, VD
	triangulateio		m_tDT_in, m_tDT_out, m_tVD_out;

	string				m_strCurrentTriangleMode = "Default";
	TRIMODE				m_eCurrentTriangleMode = TRIMODE::DEFAULT;

	_int				m_iPointCount = 0;
	vector<Obst>		m_vecObstacles;
	vector<Vec3>		m_vecObstaclePoints;
	vector<const _char*>m_strObstaclePoints;

	vector<Vec3>		m_vecRegions;

	///////////////////////////////////////////////////

	wstring				m_strPickedObject;
	CGameObject*		m_pPickedObject = nullptr;
	CShader*			m_pShader = nullptr;

	_int				m_Item_Current = 0;
	wstring				m_strFilePath = TEXT("MainStageNavMesh");

	vector<CellData*>	m_vecCells;
	vector<_char*>		m_strCells;

	vector<Vec3>		m_vecPoints;
	vector<const _char*>m_strPoints;
	_int				m_Point_Current;

	vector<BoundingSphere> m_vecPointSpheres;
	vector<BoundingSphere> m_vecObstaclePointSpheres;
	vector<BoundingSphere> m_vecRegionSpheres;

	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect*		m_pEffect = nullptr;
	ID3D11InputLayout*	m_pInputLayout = nullptr;

public:
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END