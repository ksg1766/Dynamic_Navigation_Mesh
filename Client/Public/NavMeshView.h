#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;
class CShader;

END

struct VDPoint
{
	_int x, y;
};

BEGIN(Client)

class CNavMeshView final : public CView
{
    using Super = CView;
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

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
	HRESULT	CreateVoronoi();

private:
	HRESULT	DebugRenderLegacy();

private:
	_bool	CanClimb();

	void	Input();
	_bool	Pick(_uint screenX, _uint screenY);

	HRESULT	Save();
	HRESULT	Load();

private:
	void	InfoView();
	void	PointGroup();
	void	CellGroup();

private:
	_bool				m_isNavMeshOn = false;
	_float				m_fSlopeDegree = 0.0f;
	_float				m_fMaxClimb = 0.0f;
	const _float		m_fEpsilon = 0.001f;
	_float				m_fMinArea = 0.0f;

	BoundingBox			m_tNavMeshBoundVolume;

	CTerrain*			m_pTerrainBuffer = nullptr;

	// VD
	vector<VDPoint>		m_vecVDCaches;
	vector<Vec3>		m_vecVDPoints;

	///////////////////////////////////////////////////

	wstring				m_strPickedObject;
	CGameObject*		m_pPickedObject = nullptr;
	CShader*			m_pShader = nullptr;

	_int				m_Item_Current = 0;
	wstring				m_strFilePath = TEXT("MainStageNavMesh");

	vector<CellData*>	m_vecCells;
	vector<_char*>		m_strCells;
	// cache
	vector<Vec3>		m_vecPoints;
	vector<const _char*>m_strPoints;
	_int				m_Point_Current;

	vector<BoundingSphere*> m_vecSphere;

	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect*		m_pEffect = nullptr;
	ID3D11InputLayout*	m_pInputLayout = nullptr;

public:
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END