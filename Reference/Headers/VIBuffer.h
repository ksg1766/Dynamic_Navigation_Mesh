#pragma once

#include "Component.h"

/* CVIBuffer : Vertices + Indices */
/* 정점과 인덱스를 할당하고 초기화한다. */
/* 렌더함수에서  이 두 버퍼를 이용하여 그린다.  */
BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
	using Super = CComponent;
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	_float3*					m_pVerticesPos = nullptr;
	//
	_uint						m_iNumVBs = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iStride = { 0 }; /* 정점하나의 크기(Byte) */

	// For Cube
	_uint						m_iNumPrimitives = 0;
	_uint						m_iIndexSizeofPrimitive = 0;
	_uint						m_iNumIndicesofPrimitive = 0;

	DXGI_FORMAT					m_eIndexFormat;

protected:
	HRESULT Create_Buffer(_Inout_ ID3D11Buffer** ppOut);

public:
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) = 0;
	virtual void Free() override;
};

END