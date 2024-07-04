#include "..\Public\Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Channel.h"
#include "Animation.h"
#include "GameObject.h"
#include "GraphicDevice.h"
#include "LevelManager.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_BoneIndices(rhs.m_BoneIndices)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMesh::Initialize_Prototype(string& strName, vector<VTXMESH>& Vertices, vector<_int>& Indices,
	_uint iMatIndex, vector<_int>& Bones, Matrix& matPivot, CModel* pModel, vector<Vec3>& vecSurfaceVtx, vector<FACEINDICES32>& vecSurfaceIdx)
{
	/* 이 메시와 이름이 같은 뼈가 존재한다면. 이 뼈의 행렬을 메시를 구성하는 정점에 곱해질 수 있도록 유도한다. */
	strcpy_s(m_szName, strName.c_str());

	m_iMaterialIndex = iMatIndex;

	if (FAILED(Ready_StaticVertices(Vertices, matPivot, vecSurfaceVtx)))
		return E_FAIL;

	if (FAILED(Ready_Indices(Indices, &vecSurfaceIdx)))
		return E_FAIL;

	/* 이 메시에서 사용하는 본의 인덱스를 저장한다. */
	m_BoneIndices.reserve((_uint)Bones.size());
	for (_int index : Bones)
		m_BoneIndices.push_back(index);

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(string& strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices,
	_uint iMatIndex, vector<_int>& Bones, CModel* pModel)
{
	strcpy_s(m_szName, strName.c_str());

	m_iMaterialIndex = iMatIndex;

	if (FAILED(Ready_AnimVertices(Vertices)))
		return E_FAIL;

	if (FAILED(Ready_Indices(Indices)))
		return E_FAIL;

	/* 이 메시에서 사용하는 본의 인덱스를 저장한다. */
	m_BoneIndices.reserve((_uint)Bones.size());
	for (_int index : Bones)
		m_BoneIndices.push_back(index);

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	/* 프로토타입에서 저장한 본 인덱스를 바탕으로 모델에서 본을 가져온다. */
	if (nullptr != pArg)
	{
		CModel* pModel = static_cast<CModel*>(pArg);

		for (size_t i = 0; i < m_BoneIndices.size(); i++)
		{
			CBone* pBone = pModel->GetBone(m_BoneIndices[i]);
			if (nullptr != pBone)
			{
				m_Bones.push_back(pBone);
				Safe_AddRef(pBone);
			}
		}

		m_BoneIndices.clear();
		m_BoneIndices.shrink_to_fit();
	}

	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_Bones.size())
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		/* 셰이더에 행렬 던질 때는 전치 꼭 */
		/* 최종 트랜스폼 계산*/

		XMStoreFloat4x4(&pBoneMatrices[i], m_Bones[i]->Get_OffSetMatrix() * m_Bones[i]->Get_CombinedTransformation() * PivotMatrix);
	}
}

HRESULT CMesh::Ready_StaticVertices(vector<VTXMESH>& Vertices, _fmatrix& PivotMatrix, vector<Vec3>& vecSurfaceVtx)
{
	m_iNumVBs = 1;
	m_iNumVertices = (_int)Vertices.size();
	m_iNumPrimitives = m_iNumVertices / 3;
	m_iNumIndicesofPrimitive = 3;
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//CLevelManager* pLevelManager = GET_INSTANCE(CLevelManager);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &Vertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &Vertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexcoord, &Vertices[i].vTexcoord, sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &Vertices[i].vTangent, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));

		//if(3 == pLevelManager->GetCurrentLevelIndex())
			vecSurfaceVtx.push_back(pVertices[i].vPosition);
	}

	//RELEASE_INSTANCE(CLevelManager);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_AnimVertices(vector<VTXANIMMESH>& Vertices)
{
	/* 기본 정점 버퍼 세팅 */
	m_iNumVBs = 1;
	m_iNumVertices = (_int)Vertices.size();
	m_iNumPrimitives = m_iNumVertices / 3;
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		/* NoneAnim 모델과 달리 사전변환을 진행하지 않는다. */
		memcpy(&pVertices[i].vPosition, &Vertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &Vertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &Vertices[i].vTexture, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Vertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndex, &Vertices[i].vBlendIndex, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeight, &Vertices[i].vBlendWeight, sizeof(Vec4));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Indices(vector<_int>& Indices, vector<FACEINDICES32>* vecSurfaceIdx)
{
	m_iNumPrimitives = ((_int)Indices.size()) / 3;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	::ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitives];
	::ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	FACEINDICES32* pIndicesCache = new FACEINDICES32[m_iNumPrimitives];
	::ZeroMemory(pIndicesCache, sizeof(FACEINDICES32) * m_iNumPrimitives);

	size_t IndicesOffset = 0;
	if (vecSurfaceIdx)
		IndicesOffset = vecSurfaceIdx->size() * 3;

	//CLevelManager* pLevelManager = GET_INSTANCE(CLevelManager);

	for (_uint i = 0, j = 0; i < m_iNumPrimitives; ++i, ++j)
	{
		pIndices[i]._0 = Indices[j];
		pIndices[i]._1 = Indices[++j];
		pIndices[i]._2 = Indices[++j];
		
		if (vecSurfaceIdx)
		{
			pIndicesCache[i]._0 = pIndices[i]._0 + IndicesOffset;
			pIndicesCache[i]._1 = pIndices[i]._1 + IndicesOffset;
			pIndicesCache[i]._2 = pIndices[i]._2 + IndicesOffset;

			//if (3 == pLevelManager->GetCurrentLevelIndex())
				vecSurfaceIdx->push_back(pIndicesCache[i]);
		}
	}

	//RELEASE_INSTANCE(CLevelManager);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string& strName,
	vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, Matrix& PivotMatrix, CModel* pModel, vector<Vec3>& vecSurfaceVtx, vector<FACEINDICES32>& vecSurfaceIdx)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strName, Vertices, Indices, iMatIndex, Bones, PivotMatrix, pModel, vecSurfaceVtx, vecSurfaceIdx)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string& strName,
	vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, CModel* pModel)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strName, Vertices, Indices, iMatIndex, Bones, pModel)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(CGameObject* pGameObject, void* pArg)
{
	CMesh* pInstance = new CMesh(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	Super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();
}
