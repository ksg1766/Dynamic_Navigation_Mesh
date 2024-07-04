#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CLayer;
class CGameObject;

END

BEGIN(Client)

class CLayersView : public CView
{
    using Super = CView;
private:
	CLayersView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLayersView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

public:
	CGameObject* SelectedObject() { return m_pCurPickedObject; }

private:
	void		Input();
	CGameObject* Pick(_int screenX, _int screenY);

private:
	void	InfoView();
	void	TreeGroups();
	void	TapGroups(_uint i);

private:
	LAYERTAG			m_ePickedLayerTag = LAYERTAG::LAYER_END;
	wstring				m_strPickedObject;
	CGameObject*		m_pCurPickedObject = nullptr;
	CGameObject*		m_pPrePickedObject = nullptr;

	_int				m_Item_Current = 0;

	class CTransformView*	m_pTransfoemView = nullptr;//

public:
	static class CLayersView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END