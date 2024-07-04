#pragma once
#include "Base.h"
#include "Hasher.h"

BEGIN(Engine)

class CShader;
class CVIBuffer_Rect;
class CRenderTarget;
class CTargetManager final : public CBase
{
	using Super = CBase;
    DECLARE_SINGLETON(CTargetManager);

private:
	CTargetManager();
	virtual ~CTargetManager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Bind_SRV(CShader* pShader, const wstring& strTargetTag, const _char* pConstantName);

	/* strMRTTag�� �ش��ϴ� list�� ����ִ� Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);

	/* �ٽ� ���� ���·� �����Ѵ�. */
	HRESULT End_MRT(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDSV = nullptr);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif
private:
	CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	vector<CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

private:
	unordered_map<const wstring, CRenderTarget*, djb2Hasher>			m_RenderTargets;

	/* ��ġ�� ���ÿ� ���ε��Ǿ���ϴ� Ÿ�ٵ��� �̸� ����ΰڴ�. */
	unordered_map<const wstring, vector<CRenderTarget*>, djb2Hasher>	m_MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };

public:
	virtual void Free() override;
};

END