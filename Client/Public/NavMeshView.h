#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;
class CShader;

END

BEGIN(Client)

class CNavMeshView final : public CView
{
    using Super = CView;
public:

private:
	CNavMeshView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNavMeshView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

private:
	void	ShowNavMesh(_bool bOnOff) {	m_isNavMeshOn = bOnOff; }

	HRESULT	BakeNavMesh();

private:
	HRESULT	DebugRenderLegacy();

private:
	void	Input();
	_bool	Pick(_uint screenX, _uint screenY);

	HRESULT	Save();
	HRESULT	Load();

private:
	void	InfoView();
	void	PointGroup();
	void	CellGroup();

private:
	struct CellData;

	_bool				m_isNavMeshOn = false;
	_float				m_fSlopeDegree = 0.f;
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
	static class CNavMeshView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END