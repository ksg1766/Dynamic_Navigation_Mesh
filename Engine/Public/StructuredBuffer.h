#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CStructuredBuffer : public CBase
{
	using Super = CBase;
private:
	CStructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStructuredBuffer(const CStructuredBuffer& rhs);
	virtual ~CStructuredBuffer() = default;

public:
	HRESULT Initialize(void* pInputData, _uint iInputStride, _uint iInputCount, _uint iOutputStride = 0, _uint iOutputCount = 0);

private:
	HRESULT Create_Buffer();
	HRESULT CreateInput();
	HRESULT CreateSRV();
	HRESULT CreateOutput();
	HRESULT CreateUAV();
	HRESULT CreateResult();

public:
	_uint GetInputByteWidth()	{ return m_iInputStride * m_iInputCount; }
	_uint GetOutputByteWidth()	{ return m_iOutputStride * m_iOutputCount; }

	HRESULT CopyToInput(void* data);
	HRESULT CopyFromOutput(void* data);

public:
	ID3D11ShaderResourceView*	GetSRV()	{ return m_pSRV; }
	ID3D11UnorderedAccessView*	GetUAV()	{ return m_pUAV; }

private:
	ID3D11Buffer*				m_pInput;
	ID3D11ShaderResourceView*	m_pSRV; // Input
	ID3D11Buffer*				m_pOutput;
	ID3D11UnorderedAccessView*	m_pUAV; // Output
	ID3D11Buffer*				m_pResult;

	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

private:
	void* m_pIinputData;
	_uint m_iInputStride = 0;
	_uint m_iInputCount = 0;
	_uint m_iOutputStride = 0;
	_uint m_iOutputCount = 0;

public:
	static CStructuredBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pInputData, _uint iInputStride, _uint iInputCount, _uint iOutputStride = 0, _uint iOutputCount = 0);
	virtual void Free() override;
};

END