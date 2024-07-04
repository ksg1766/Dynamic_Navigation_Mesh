#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell : public CBase
{
	using Super = CBase;
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCell(const CCell& rhs);
	virtual ~CCell() = default;
	
public:
	const _float3*	Get_Point(POINTS ePoint) const				{ return &m_vPoints[ePoint]; }
	const _float3*	Get_Points() const							{ return m_vPoints; }

	void			SetUp_Neighbor(LINE eLine, CCell* pCell)	{ m_iNeighborIndices[eLine] = pCell->m_iIndex; }

	HRESULT			Initialize(const _float3* pPoints, _uint iIndex);
	void			DebugRender(PrimitiveBatch<VertexPositionColor>*& pBatch, XMVECTORF32 vColor);

	_bool			Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool			isOut(_fvector vPoint, _int* pNeighborIndex);
	_float3			GetPassedEdgeNormal(_fvector vPoint);

private:
	_uint					m_iIndex = {};
	_float3					m_vPoints[POINT_END];
	_float3					m_vNormals[LINE_END];

	_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

#ifdef _DEBUG
	//class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END