#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext, ComponentType::Texture)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_ppSRVs(rhs.m_ppSRVs)
{
	for (size_t i = 0; i < m_iNumTextures; i++)	
		Safe_AddRef(m_ppSRVs[i]);
	
}

HRESULT CTexture::Initialize_Prototype(const wstring & strTextureFilePath, _uint iNumTextures)
{
	/* ../Bin/Resources/Textures/Explosion%d.png */

	m_iNumTextures = iNumTextures;

	m_ppSRVs = new ID3D11ShaderResourceView*[iNumTextures];

	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		/* .dds, .png, .jpg, .bmp */		
		_tchar		szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT		hr = 0;
		ID3D11ShaderResourceView*		pSRV = nullptr;

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if(false == lstrcmp(szExt, TEXT(".tga")))
		{
			return E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			int a = 0;//return E_FAIL;

		m_ppSRVs[i] = pSRV;
	}


	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(const CShader * pShader, const _char * pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_Resource(pConstantName, m_ppSRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(const CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Textures(pConstantName, m_ppSRVs, m_iNumTextures);
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strTextureFilePath, _uint iNumTextures)
{
	CTexture*	pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(CGameObject* pGameObject, void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumTextures; i++)
		Safe_Release(m_ppSRVs[i]);

	if(false == m_isCloned)
		Safe_Delete_Array(m_ppSRVs);	
}