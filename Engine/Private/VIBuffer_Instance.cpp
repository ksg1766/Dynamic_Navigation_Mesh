#include "..\Public\VIBuffer_Instance.h"
#include "VIBuffer_Point.h"
#include "Mesh.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: Super(pDevice, pContext)
{

}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: Super(rhs)
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	if (FAILED(Create_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Render(CVIBuffer_Point* pPoint)
{
	ID3D11Buffer* pVertexBuffers[] = {
		pPoint->m_pVB,
	};

	_uint			iStrides[] = {
		pPoint->m_iStride,
	};

	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, pPoint->m_iNumVBs, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(pPoint->m_pIB, pPoint->m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(pPoint->m_eTopology);

	const _uint dataCount = static_cast<_uint>(m_vecData.size());

	D3D11_MAPPED_SUBRESOURCE subResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		::memcpy(subResource.pData, m_vecData.data(), sizeof(InstancingData) * dataCount);
	}
	m_pContext->Unmap(m_pVB, 0);

	m_pContext->IASetVertexBuffers(1, 1, &m_pVB, &m_iStride, iOffsets);

	m_pContext->DrawIndexedInstanced(pPoint->m_iNumIndicesofPrimitive * pPoint->m_iNumPrimitives, dataCount, 0, 0, 0U);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render(CMesh* pMesh)
{
	ID3D11Buffer*	pVertexBuffers[] = {
		pMesh->m_pVB,
	};

	_uint			iStrides[] = {
		pMesh->m_iStride,
	};

	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, pMesh->m_iNumVBs, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(pMesh->m_pIB, pMesh->m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(pMesh->m_eTopology);

	const _uint dataCount = static_cast<_uint>(m_vecData.size());

	D3D11_MAPPED_SUBRESOURCE subResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		::memcpy(subResource.pData, m_vecData.data(), sizeof(InstancingData) * dataCount);
	}
	m_pContext->Unmap(m_pVB, 0);

	m_pContext->IASetVertexBuffers(1, 1, &m_pVB, &m_iStride, iOffsets);

	m_pContext->DrawIndexedInstanced(pMesh->m_iNumIndicesofPrimitive * pMesh->m_iNumPrimitives, dataCount, 0, 0, 0U);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Create_Buffer(_uint iMaxCount)
{
	m_iMaxCount = iMaxCount;
	vector<InstancingData> temp(m_iMaxCount);

	m_iStride = sizeof(InstancingData);
	//m_iNumVertices = 4;
	m_iNumVBs = 1;

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = (_uint)(m_iStride * temp.size());
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	//m_BufferDesc.StructureByteStride = m_iStride; //

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = temp.data();

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//TODO:잘 고치자
	// 
	//if (nullptr == m_pDevice)
	//	return E_FAIL;

	//return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, ppOut);	
	return S_OK;
}

void CVIBuffer_Instance::ClearData()
{
	m_vecData.clear();
}

void CVIBuffer_Instance::AddData(InstancingData& data)
{
	m_vecData.push_back(data);
}

CVIBuffer_Instance* CVIBuffer_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instance* pInstance = new CVIBuffer_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance::Free()
{
	Super::Free();
}
