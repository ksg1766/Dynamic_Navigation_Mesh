#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrain final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			InitializeJustGrid(const _uint& iSizeX, const _uint& iSizeZ, const _uint iCX = 64, const _uint iCZ = 64);
	HRESULT			InitializeNorTex(const _uint& iSizeX, const _uint& iSizeZ, const _uint iCX = 64, const _uint iCZ = 64);
	HRESULT			InitializeWithHeightMap(const wstring& strHeightMapPath);
	void			DebugRender()	override;

public:
	_bool			Pick(_uint screenX, _uint screenY, Vec3& pickPos, _float& distance, const Matrix& matWorld);

private:
	_ulong				m_iNumVerticesX = { 0 };
	_ulong				m_iNumVerticesZ = { 0 };
	FACEINDICES32*		m_pFaceIndices = nullptr;
	ID3D11InputLayout*	m_pInputLayout = nullptr;

private:
	vector<Vec3>		m_vecVerticesCache;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	class BasicEffect* m_pEffect = nullptr;
#endif

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END