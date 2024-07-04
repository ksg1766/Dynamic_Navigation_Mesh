#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Begin();
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix) const;
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices) const;
	HRESULT Bind_Texture(const _char* pConstantName, ID3D11ShaderResourceView* pSRV) const;
	HRESULT Bind_Textures(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures) const;
	HRESULT Dispatch(UINT iPass, UINT iX, UINT iY, UINT iZ);

	HRESULT Get_RawValue(const _char* pConstantName, OUT void* pData, _uint iLength) const;
	HRESULT Get_Matrix(const _char* pConstantName, OUT _matrix* pMatrix) const;
	HRESULT Get_Matrices(const _char* pConstantName, OUT _float4x4* pMatrices, _uint iNumMatrices) const;
	HRESULT Get_UAV(const _char* pConstantName, OUT void* pData) const;

	_int	GetPassIndex()					{ return m_iPassIndex; }
	void	SetPassIndex(_int iPassIndex)	{ m_iPassIndex = iPassIndex; }

private:	
	ID3DX11Effect*				m_pEffect = { nullptr };
	vector<ID3D11InputLayout*>	m_InputLayouts; /* 각 패스마다 인풋레이아웃을 만들어서 추가했다. */

	_int						m_iPassIndex = 0;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, 
		const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool pArg = false);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END