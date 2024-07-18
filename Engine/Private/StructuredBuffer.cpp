#include "..\Public\StructuredBuffer.h"

CStructuredBuffer::CStructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CStructuredBuffer::CStructuredBuffer(const CStructuredBuffer& rhs)
	: Super(rhs)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CStructuredBuffer::Initialize(void* pInputData, _uint iInputStride, _uint iInputCount, _uint iOutputStride, _uint iOutputCount)
{
	m_pIinputData = pInputData;
	m_iInputStride = iInputStride;
	m_iInputCount = iInputCount;
	m_iOutputStride = iOutputStride;
	m_iOutputCount = iOutputCount;

	if (0 == iOutputStride || 0 == iOutputCount)
	{
		m_iOutputStride = iInputStride;
		m_iOutputCount = iInputCount;
	}

	if (FAILED(Create_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::Create_Buffer()
{
	if (FAILED(CreateInput())||
		FAILED(CreateSRV())||
		FAILED(CreateOutput())||
		FAILED(CreateUAV())||
		FAILED(CreateResult()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateInput()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ByteWidth = GetInputByteWidth();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_iInputStride;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = m_pIinputData;

	if (nullptr != m_pIinputData)
	{
		if (FAILED((m_pDevice->CreateBuffer(&desc, &subResource, &m_pInput))))
			return E_FAIL;
	}
	else
	{
		if (FAILED((m_pDevice->CreateBuffer(&desc, nullptr, &m_pInput))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStructuredBuffer::CreateSRV()
{
	D3D11_BUFFER_DESC desc;
	m_pInput->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.NumElements = m_iInputCount;

	if (FAILED((m_pDevice->CreateShaderResourceView(m_pInput, &srvDesc, &m_pSRV))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateOutput()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ByteWidth = GetOutputByteWidth();
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_iOutputStride;

	if (FAILED((m_pDevice->CreateBuffer(&desc, nullptr, &m_pOutput))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateUAV()
{
	D3D11_BUFFER_DESC desc;
	m_pOutput->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = m_iOutputCount;

	if (FAILED((m_pDevice->CreateUnorderedAccessView(m_pOutput, &uavDesc, &m_pUAV))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CreateResult()
{
	D3D11_BUFFER_DESC desc;
	m_pOutput->GetDesc(&desc);

	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED((m_pDevice->CreateBuffer(&desc, NULL, &m_pResult))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructuredBuffer::CopyToInput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	if(FAILED(m_pContext->Map(m_pInput, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
		return E_FAIL;

	memcpy(subResource.pData, data, GetInputByteWidth());

	m_pContext->Unmap(m_pInput, 0);
	return S_OK;
}

HRESULT CStructuredBuffer::CopyFromOutput(void* data)
{
	m_pContext->CopyResource(m_pResult, m_pOutput);

	D3D11_MAPPED_SUBRESOURCE subResource;
	if (FAILED(m_pContext->Map(m_pResult, 0, D3D11_MAP_READ, 0, &subResource)))
		return E_FAIL;

	memcpy(data, subResource.pData, GetOutputByteWidth());

	m_pContext->Unmap(m_pResult, 0);
	return S_OK;
}

CStructuredBuffer* CStructuredBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	void* pInputData, _uint iInputStride, _uint iInputCount, _uint iOutputStride, _uint iOutputCount)
{
	CStructuredBuffer* pInstance = new CStructuredBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pInputData, iInputStride, iInputCount, iOutputStride, iOutputCount)))
	{
		MSG_BOX("Failed to Created : CStructuredBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStructuredBuffer::Free()
{
	Super::Free();
	Safe_Release(m_pInput);
	Safe_Release(m_pSRV);
	Safe_Release(m_pOutput);
	Safe_Release(m_pUAV);
	Safe_Release(m_pResult);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
