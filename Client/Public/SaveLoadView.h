#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

END

BEGIN(Client)

class CSaveLoadView : public CView
{
    using Super = CView;
private:
	CSaveLoadView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSaveLoadView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

private:
	void	Input();

	HRESULT	Save();
	HRESULT	Load();
	HRESULT	Clear();

private:
	void	InfoView();
	void	TreeGroups();

private:
	_int	m_Item_Current = 0;
	wstring	m_strFilePath;
	_bool	m_arrLayerTable[static_cast<_uint>(LAYERTAG::LAYER_END)] = { false };

	_bool	m_bSaveNewButton = false;
	_char	m_szFileNameBuf[128] = "";

public:
	static class CSaveLoadView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END