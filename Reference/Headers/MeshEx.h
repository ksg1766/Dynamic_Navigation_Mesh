#pragma once

/* 모델의 특정 파츠를 의미하는 메시. */
/* 각 파츠(메시)마다 정점, 인덱스버퍼를 가진다. */
#include "VIBuffer.h"
#include "FileUtils.h"
#include "ModelEx.h"

BEGIN(Engine)

class CMeshEx final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CMeshEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshEx(const CMeshEx& rhs);
	virtual ~CMeshEx() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT	LoadData_FromMeshFile(CModelEx::TYPE eModelType, FileUtils* pFileUtils, Matrix PivotMatrix);
	HRESULT	LoadData_FromConverter(CModelEx::TYPE eModelType, shared_ptr<asMesh> pMesh, Matrix PivotMatrix);

private:
	//char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */

	_uint							m_iBoneIndex;
	wstring							m_szName;
	wstring							m_szMaterialName;
private:
	HRESULT Ready_Vertices(VTXANIMMODEL* pVertices, Matrix PivotMatrix);

public:
	static CMeshEx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg = nullptr) override;
	virtual void Free() override;
};

END