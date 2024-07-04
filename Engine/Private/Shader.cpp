#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext, ComponentType::Shader)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool pArg)
{
	_uint		iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif	

	if (pArg) iHlslFlag |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

	/* 해당 경로에 셰이더 파일을 컴파일하여 객체화한다. */
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* 이 셰이더안에 있는 모든 패스가 내가 그릴려고하는 정점을 잘 받아줄 수 있는가?! 확인. 이게 확인되면 */
	/* ID3D11InputLayout을 생성해 준다. */
	/* 결론적으로 ID3D11InputLayout을 생성해주기위해 밑에 일을 한다 .*/

	/* 패스를 얻기위한 작업을 선행한다. 패스는 테크니커 안에 선언되어있다. */
	/* 이 셰이더 안에 정의된 테크니커의 정볼르 얻어온다. (나는 테크니커를 하나만 정의했기때문에 0번째 인덱스로 받아온거여. )*/
	ID3DX11EffectTechnique*			pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;
	
	/* 테크니커의 정보를 얻어온다. */
	D3DX11_TECHNIQUE_DESC			TechniqueDesc;	
	pTechnique->GetDesc(&TechniqueDesc);

	/* TechniqueDesc.Passes : 패스의 갯수. */	
	for (_uint i = 0; i < TechniqueDesc.Passes; i++)
	{
		/* pTechnique->GetPassByIndex(i) : i번째 패스객체를 얻어온다. */
		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);

		/* 패스의 정보를 얻어온다. */
		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);

		ID3D11InputLayout*			pInputLayout = { nullptr };

		/* 패스안에 선언된 정점정보와 내가 던져준 정점정보가 일치한다면 ID3D11InputLayout를 생성해준다. */
		if (iNumElements > 0)
		{
			if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
				return E_FAIL;

			m_InputLayouts.push_back(pInputLayout);
		}
	}

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin()
{
	if (m_iPassIndex >= m_InputLayouts.size())
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[m_iPassIndex]);

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	/* iPassIndex번째 패스로 그리기를 시작합니다. */
	pTechnique->GetPassByIndex(m_iPassIndex)->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix) const
{
	/* pConstantName이름에 해당하는 타입을 고려하지않은 전역변수를 컨트롤하는 객체를 얻어온다 .*/
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*		pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((const _float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char * pConstantName, const _float4x4* pMatrices, _uint iNumMatrices) const
{
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;	

	ID3DX11EffectMatrixVariable*		pMatrix = pVariable->AsMatrix();
	if (nullptr == pMatrix)
		return E_FAIL;

	return pMatrix->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader::Bind_Texture(const _char * pConstantName, ID3D11ShaderResourceView* pSRV) const
{
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);	
}

HRESULT CShader::Bind_Textures(const _char * pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures) const
{
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRVs, 0, iNumTextures);	
}

HRESULT CShader::Dispatch(UINT iPass, UINT iX, UINT iY, UINT iZ)
{
	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPass)->Apply(0, m_pContext);
	m_pContext->Dispatch(iX, iY, iZ);

	ID3D11ShaderResourceView* null[1] = { 0 };
	m_pContext->CSSetShaderResources(0, 1, null);

	ID3D11UnorderedAccessView* nullUav[1] = { 0 };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUav, NULL);

	m_pContext->CSSetShader(NULL, NULL, 0);

	return S_OK;
}

HRESULT CShader::Get_RawValue(const _char* pConstantName, OUT void* pData, _uint iLength) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->GetRawValue(pData, 0, iLength);
}

HRESULT CShader::Get_Matrix(const _char* pConstantName, OUT _matrix* pMatrix) const
{
	/* pConstantName이름에 해당하는 타입을 고려하지않은 전역변수를 컨트롤하는 객체를 얻어온다 .*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	if (FAILED(pMatrixVariable->GetMatrix((_float*)pMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Get_Matrices(const _char* pConstantName, OUT _float4x4* pMatrices, _uint iNumMatrices) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrix = pVariable->AsMatrix();
	if (nullptr == pMatrix)
		return E_FAIL;

	return pMatrix->GetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader::Get_UAV(const _char* pConstantName, OUT void* pData) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectUnorderedAccessViewVariable* pUAV = pVariable->AsUnorderedAccessView();
	if (nullptr == pUAV)
		return E_FAIL;

	return pUAV->SetUnorderedAccessView((ID3D11UnorderedAccessView*)pData);
}

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, _bool pArg)
{
	CShader*	pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements, pArg)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(CGameObject* pGameObject, void * pArg)
{
	CShader*	pInstance = new CShader(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	Safe_Release(m_pEffect);
}
