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

	/* �ش� ��ο� ���̴� ������ �������Ͽ� ��üȭ�Ѵ�. */
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* �� ���̴��ȿ� �ִ� ��� �н��� ���� �׸������ϴ� ������ �� �޾��� �� �ִ°�?! Ȯ��. �̰� Ȯ�εǸ� */
	/* ID3D11InputLayout�� ������ �ش�. */
	/* ��������� ID3D11InputLayout�� �������ֱ����� �ؿ� ���� �Ѵ� .*/

	/* �н��� ������� �۾��� �����Ѵ�. �н��� ��ũ��Ŀ �ȿ� ����Ǿ��ִ�. */
	/* �� ���̴� �ȿ� ���ǵ� ��ũ��Ŀ�� ������ ���´�. (���� ��ũ��Ŀ�� �ϳ��� �����߱⶧���� 0��° �ε����� �޾ƿ°ſ�. )*/
	ID3DX11EffectTechnique*			pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;
	
	/* ��ũ��Ŀ�� ������ ���´�. */
	D3DX11_TECHNIQUE_DESC			TechniqueDesc;	
	pTechnique->GetDesc(&TechniqueDesc);

	/* TechniqueDesc.Passes : �н��� ����. */	
	for (_uint i = 0; i < TechniqueDesc.Passes; i++)
	{
		/* pTechnique->GetPassByIndex(i) : i��° �н���ü�� ���´�. */
		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);

		/* �н��� ������ ���´�. */
		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);

		ID3D11InputLayout*			pInputLayout = { nullptr };

		/* �н��ȿ� ����� ���������� ���� ������ ���������� ��ġ�Ѵٸ� ID3D11InputLayout�� �������ش�. */
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

	/* iPassIndex��° �н��� �׸��⸦ �����մϴ�. */
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
	/* pConstantName�̸��� �ش��ϴ� Ÿ���� ����������� ���������� ��Ʈ���ϴ� ��ü�� ���´� .*/
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
	/* pConstantName�̸��� �ش��ϴ� Ÿ���� ����������� ���������� ��Ʈ���ϴ� ��ü�� ���´� .*/
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
