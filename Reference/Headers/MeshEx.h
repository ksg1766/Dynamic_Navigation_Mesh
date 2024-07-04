#pragma once

/* ���� Ư�� ������ �ǹ��ϴ� �޽�. */
/* �� ����(�޽�)���� ����, �ε������۸� ������. */
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
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */

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