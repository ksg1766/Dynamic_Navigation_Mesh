#include "..\Public\VIBuffer_Dot.h"

CVIBuffer_Dot::CVIBuffer_Dot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Dot::CVIBuffer_Dot(const CVIBuffer_Dot& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Dot::Initialize_Prototype()
{
	m_iStride = sizeof(VTXPOS); /* 정점하나의 크기 .*/
	m_iNumVertices = 1;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOS*		pVertices = new VTXPOS;
	ZeroMemory(pVertices, sizeof(VTXPOS));

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	//m_iIndexStride = 2; /* 인덱스 하나의 크기. 2 or 4 */
	//m_iNumIndices = 6;
	//m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumPrimitives = 1;
	m_iIndexSizeofPrimitive = sizeof(_ushort);
	m_iNumIndicesofPrimitive = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iIndexSizeofPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iIndexSizeofPrimitive);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Dot::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Dot * CVIBuffer_Dot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Dot*	pInstance = new CVIBuffer_Dot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Dot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Dot::Clone(CGameObject* pGameObject, void * pArg)
{
	CVIBuffer_Dot*	pInstance = new CVIBuffer_Dot(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Dot");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Dot::Free()
{
	Super::Free();


}