#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{
	using Super = CVIBuffer;
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	HRESULT Render(class CVIBuffer_Point* pPoint);
	HRESULT Render(class CMesh* pMesh);

protected:
	//ID3D11Buffer* m_pVB = { nullptr };
	//ID3D11Buffer* m_pIB = { nullptr };

	//D3D11_BUFFER_DESC			m_BufferDesc;
	//D3D11_SUBRESOURCE_DATA	m_SubResourceData;

	//D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	//_float3*					m_pVerticesPos = nullptr;
	////
	//_uint						m_iNumVBs = { 0 };
	//_uint						m_iNumVertices = { 0 };
	//_uint						m_iStride = { 0 }; /* 정점하나의 크기(Byte) */

	//// For Cube
	//_uint						m_iNumPrimitives = 0;
	//_uint						m_iIndexSizeofPrimitive = 0;
	//_uint						m_iNumIndicesofPrimitive = 0;

	//DXGI_FORMAT				m_eIndexFormat;

private:
	HRESULT Create_Buffer(_uint iMaxCount = MAX_INSTANCE);

public:
	void ClearData();
	void AddData(InstancingData& data);
	vector<InstancingData>& GetInstanceData() {	return m_vecData; }

protected:
	_uint						m_iMaxCount = 0;
	vector<InstancingData>		m_vecData;

public:
	static CVIBuffer_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) { return nullptr; };
	virtual void Free() override;
};

END