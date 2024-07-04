#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
	using Super = CVIBuffer_Instance;
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Render()				override;

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
	ID3D11Buffer*	m_pVBPoint = { nullptr };
	_uint			m_iStridePoint = { 0 }; /* 정점하나의 크기(Byte) */

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END