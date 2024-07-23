#include "..\Public\VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	_float fRadius = 0.5f; // 구의 반지름
	_uint iStackCount = 20; // 가로 분할
	_uint iSliceCount = 20; // 세로 분할

#pragma region VERTEXBUFFER
	m_iNumVBs = 1;
	m_iNumVertices = iStackCount * (iSliceCount + 1) + 2;
	m_iStride = sizeof(VTXNORTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXNORTEX* pVertices = new VTXNORTEX[iStackCount * (iSliceCount + 1) + 2];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * iStackCount * (iSliceCount + 1) + 2);

	pVertices[0].vPosition = Vec3(0.0f, fRadius, 0.0f);
	pVertices[0].vNormal = pVertices[0].vPosition;
	XMStoreFloat3(&pVertices[0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[0].vNormal)));
	pVertices[0].vTexcoord = Vec2(0.5f, 0.0f);

	_float fStackAngle = XM_PI / iStackCount;
	_float fSliceAngle = XM_2PI / iSliceCount;

	_float fDeltaU = 1.f / static_cast<_float>(iSliceCount);
	_float fDeltaV = 1.f / static_cast<_float>(iStackCount);

	_int iCount = 1;
	for (_int iY = 1; iY <= iStackCount - 1; ++iY)
	{
		_float fPhi = iY * fStackAngle;

		for (_int iX = 0; iX <= iSliceCount; ++iX)
		{
			_float fTheta = iX * fSliceAngle;

			pVertices[iCount].vPosition.x = fRadius * sinf(fPhi) * cosf(fTheta);
			pVertices[iCount].vPosition.y = fRadius * cosf(fPhi);
			pVertices[iCount].vPosition.z = fRadius * sinf(fPhi) * sinf(fTheta);
			
			pVertices[iCount].vNormal = pVertices[iCount].vPosition;
			XMStoreFloat3(&pVertices[iCount].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iCount].vNormal)));

			pVertices[iCount].vTexcoord.x = fDeltaU * iX;
			pVertices[iCount].vTexcoord.y = fDeltaV * iY;

			++iCount;
		}
	}

	pVertices[iCount].vPosition = Vec3(0.0f, -fRadius, 0.0f);
	pVertices[iCount].vNormal = pVertices[iCount].vPosition;
	XMStoreFloat3(&pVertices[iCount].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iCount].vNormal)));
	pVertices[iCount].vTexcoord = Vec2(0.5f, 1.0f);

	::ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 1 + 2 * (iStackCount - 1) * iSliceCount;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);
	
	iCount = 0;
	for (_uint i = 0; i <= iSliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		pIndices[i]._0 = 0;
		pIndices[i]._1 = i + 2;
		pIndices[i]._2 = i + 1;
		++iCount;
	}

	// 몸통 인덱스
	_uint iRingVertexCount = iSliceCount + 1;
	for (_uint y = 0; y < iStackCount - 2; ++y)
	{
		for (_uint x = 0; x < iSliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			pIndices[iCount]._0 = 1 + (y) * iRingVertexCount + (x);
			pIndices[iCount]._1 = 1 + (y) * iRingVertexCount + (x + 1);
			pIndices[iCount]._2 = 1 + (y + 1) * iRingVertexCount + (x);
			++iCount;

			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			pIndices[iCount]._0 = 1 + (y + 1) * iRingVertexCount + (x);
			pIndices[iCount]._1 = 1 + (y) * iRingVertexCount + (x + 1);
			pIndices[iCount]._2 = 1 + (y + 1) * iRingVertexCount + (x + 1);
			++iCount;
		}
	}

	// 남극 인덱스
	_uint iBottomIndex = iRingVertexCount + (iStackCount - 2) * iSliceCount;
	_uint iLastRingStartIndex = iBottomIndex - iRingVertexCount;
	for (_uint i = 0; i < iSliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		pIndices[iCount]._0 = iBottomIndex;
		pIndices[iCount]._1 = iLastRingStartIndex + i;
		pIndices[iCount]._2 = iLastRingStartIndex + i + 1;
		++iCount;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Sphere*	pInstance = new CVIBuffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Sphere::Clone(CGameObject* pGameObject, void * pArg)
{
	CVIBuffer_Sphere*	pInstance = new CVIBuffer_Sphere(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Sphere::Free()
{
	Super::Free();

}