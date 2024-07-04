#pragma once

/* 모델의 특정 파츠를 의미하는 메시. */
/* 각 파츠(메시)마다 정점, 인덱스버퍼를 가진다. */
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class CModel;
class ENGINE_DLL CMesh final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT Initialize_Prototype(string& strName, vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, Matrix& matPivot, CModel* pModel, vector<Vec3>& vecSurfaceVtx, vector<FACEINDICES32>& vecSurfaceIdx);
	HRESULT Initialize_Prototype(string& strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, CModel* pModel);
	virtual HRESULT Initialize(void* pArg);

public:
	_uint					Get_MaterialIndex() const	{ return m_iMaterialIndex; }

public:
	void					SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix); /* 메시의 정점을 그리기위해 셰이더에 넘기기위한 뼈행렬의 배열을 구성한다. */

private:
	_char					m_szName[MAX_PATH] = "";
	_int					m_iMaterialIndex = 0;	/* 메시를 그릴 때 사용하는 매태리얼 인덱스 */

private:
	vector<class CBone*>	m_Bones;			/* 이 메시에 영향을 주는 뼈 집합 (메시별로 렌더링시, 해당 메시에 영향을 주는 뼈 행렬을 모아서 토스 */
	vector<_int>			m_BoneIndices;		/* Cache (Initialize에서 뼈를 찾아오기 위한 인덱스들) */
	friend HRESULT CVIBuffer_Instance::Render(CMesh* pMesh);

private:
	HRESULT Ready_StaticVertices(vector<VTXMESH>& Vertices, _fmatrix& PivotMatrix, vector<Vec3>& vecSurfaceVtx);
	HRESULT Ready_AnimVertices(vector<VTXANIMMESH>& Vertices);
	HRESULT Ready_Indices(vector<_int>& Indices, vector<FACEINDICES32>* vecSurfaceIdx = nullptr);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string& strName, vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, Matrix& matPivot, CModel* pModel, vector<Vec3>& vecSurfaceVtx, vector<FACEINDICES32>& vecSurfaceIdx);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string& strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMatIndex, vector<_int>& Bones, CModel* pModel);

	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg = nullptr) override;
	virtual void Free() override;
};

END