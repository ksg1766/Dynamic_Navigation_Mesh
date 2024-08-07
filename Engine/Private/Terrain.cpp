#include "..\Public\Terrain.h"
#include "PipeLine.h"
#include "FileUtils.h"
#include "DebugDraw.h"
#include "GameObject.h"
#include "Transform.h"
#include "PipeLine.h"
#include "GraphicDevice.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: Super(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: Super(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CTerrain::Initialize_Prototype()
{
	m_eType = ComponentType::Terrain;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCodes = nullptr;
	size_t		iLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTerrain::InitializeJustGrid(_uint iSizeX, _uint iSizeZ, _uint iCX, _uint iCZ)
{
	m_iStride = sizeof(VTXPOS); /* 정점하나의 크기 .*/

	m_iNumVerticesX = iSizeX / iCX + 1;
	m_iNumVerticesZ = iSizeZ / iCZ + 1;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER

	m_pVerticesPos = new _float3[m_iNumVertices];
	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(iCX * (j - (m_iNumVerticesX - 1) / 2.f), 0.f, iCZ * (i - (m_iNumVerticesZ - 1) / 2.f));
			//m_vecVerticesCache.push_back(m_pVerticesPos[iIndex]);
		}
	}

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_uint		iNumFaces = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,		//2
				iIndex + m_iNumVerticesX + 1,	//3
				iIndex + 1,						//1
				iIndex							//0
			};

			pIndices[iNumFaces]._0 = iIndices[0];//2
			pIndices[iNumFaces]._1 = iIndices[1];//3
			pIndices[iNumFaces]._2 = iIndices[2];//1

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];//2
			pIndices[iNumFaces]._1 = iIndices[2];//1
			pIndices[iNumFaces]._2 = iIndices[3];//0

			++iNumFaces;
		}
	}
#pragma endregion

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	::ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CTerrain::InitializeNorTex(_uint iSizeX, _uint iSizeZ, _uint iCX, _uint iCZ)
{
	m_iStride = sizeof(VTXPOSTEX); /* 정점하나의 크기 .*/

	m_iNumVerticesX = iSizeX / iCX + 1;
	m_iNumVerticesZ = iSizeZ / iCZ + 1;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER

	m_pVerticesPos = new _float3[m_iNumVertices];
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(iCX * (j - (m_iNumVerticesX - 1) / 2.f), 0.f, iCZ * (i - (m_iNumVerticesZ - 1) / 2.f));
			//pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			//pVertices[iIndex].vTangent = _float3(0.f, 0.f, 0.f);
			//m_vecVerticesCache.push_back(m_pVerticesPos[iIndex]);
		}
	}

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_uint		iNumFaces = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,		//2
				iIndex + m_iNumVerticesX + 1,	//3
				iIndex + 1,						//1
				iIndex							//0
			};

			pIndices[iNumFaces]._0 = iIndices[0];//2
			pIndices[iNumFaces]._1 = iIndices[1];//3
			pIndices[iNumFaces]._2 = iIndices[2];//1

			//m_vecIndicesCache.push_back(pIndices[iNumFaces]);

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];//2
			pIndices[iNumFaces]._1 = iIndices[2];//1
			pIndices[iNumFaces]._2 = iIndices[3];//0

			//m_vecIndicesCache.push_back(pIndices[iNumFaces]);

			++iNumFaces;
		}
	}
#pragma endregion

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	::ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CTerrain::InitializeWithHeightMap(const wstring& strHeightMapPath)
{
	FileUtils* file = new FileUtils;
	file->Open(strHeightMapPath, FileMode::Read);

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;
	fh = file->Read<BITMAPFILEHEADER>();
	ih = file->Read<BITMAPINFOHEADER>();

	//_ulong* pPixel = { nullptr };
	//pPixel = new _ulong[ih.biWidth * ih.biHeight];

	void* pPixel = malloc(3 * ih.biWidth * ih.biHeight);

	file->Read(/*(void**)*/&pPixel, /*sizeof(_ulong)*/3 * ih.biWidth * ih.biHeight);

	Safe_Delete(file);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_float fDenomX = 1024.f / m_iNumVerticesX;
	_float fDenomZ = 1024.f / m_iNumVerticesZ;

	m_iStride = sizeof(VTXNORTEX); /* 정점하나의 크기 .*/
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	//m_iIndexStride = 4; /* 인덱스 하나의 크기. 2 or 4 */
	//m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	//m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	//m_iNumPrimitives = 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER

	m_pVerticesPos = new _float3[m_iNumVertices];
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			BYTE* startPtr = reinterpret_cast<BYTE*>(pPixel) + 3 * iIndex;
			m_pVerticesPos[iIndex] = _float3(fDenomX * (j - m_iNumVerticesX / 2.f), (*startPtr & 0x000000ff) / 4.f, fDenomZ * (i - m_iNumVerticesZ / 2.f));

			/*m_pVerticesPos[iIndex].x *= 0.5f;
			m_pVerticesPos[iIndex].z *= 0.5f;*/

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex];
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			//pVertices[iIndex].vTangent = _float3(0.f, 0.f, 0.f);

			m_vecVerticesCache.push_back(m_pVerticesPos[iIndex]);
		}
	}

	//Safe_Delete_Array(pPixel);
	free(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER
	/*_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = 0;*/

	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_uint		iNumFaces = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,		//5
				iIndex + m_iNumVerticesX + 1,	//6
				iIndex + 1,						//1
				iIndex							//0
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);

			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));
			
			m_vecIndicesCache.push_back(pIndices[iNumFaces]);

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);

			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			m_vecIndicesCache.push_back(pIndices[iNumFaces]);

			++iNumFaces;
		}
	}
#pragma endregion

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	::ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 정점버퍼와 인덱스 버퍼를 만든다. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

void CTerrain::DebugRender()
{
#ifdef _DEBUG
	/*m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::DrawGrid(m_pBatch, (m_iNumVerticesX - 1) / 2.f * Vec3::UnitX, (m_iNumVerticesZ - 1) / 2.f * Vec3::UnitZ, 10.f * Vec3::UnitY, (m_iNumVerticesX - 1) / 4.f, (m_iNumVerticesZ - 1) / 4.f, Colors::Aqua);

	m_pBatch->End();*/
#endif // DEBUG
}

_bool CTerrain::Pick(_uint screenX, _uint screenY, OUT Vec3& pickPos, OUT _float& distance, const Matrix& matWorld)
{
	//Matrix W = m_pGameObject->GetTransform()->WorldMatrix();
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	Matrix V = pPipeLine->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
	Matrix P = pPipeLine->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);

	CGraphicDevice* pGraphicDevice = GET_INSTANCE(CGraphicDevice);
	Viewport& vp = pGraphicDevice->GetViewPort();
	
	Vec3 n = vp.Unproject(Vec3(screenX, screenY, 0), P, V, matWorld);
	Vec3 f = vp.Unproject(Vec3(screenX, screenY, 1), P, V, matWorld);

	Vec3 start = n;
	Vec3 direction = f - n;
	direction.Normalize();

	Ray ray = Ray(start, direction);

	for (int32 i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (int32 j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndices[4] = {
				(i + 1) * m_iNumVerticesX + j,		//2
				(i + 1) * m_iNumVerticesX + j + 1,	//3
				i * m_iNumVerticesX + j + 1,		//1
				i * m_iNumVerticesX + j				//0
			};

			Vec3 p[4];
			for (int32 i = 0; i < 4; i++)
				p[i] = m_pVerticesPos[iIndices[i]];

			if (ray.Intersects(p[1], p[2], p[0], OUT distance))// 
			{
				if(isnan(distance))
					return false;

				pickPos = ray.position + ray.direction * distance;
				return true;
			}

			if (ray.Intersects(p[3], p[0], p[2], OUT distance))// 
			{
				if (isnan(distance))
					return false;

				pickPos = ray.position + ray.direction * distance;
				return true;
			}
		}
	}

	return false;
}

CTerrain* CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTerrain::Clone(CGameObject* pGameObject, void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	Super::Free();
}