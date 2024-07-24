#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CTerrain;

END

BEGIN(Client)

class CPrefabsView : public CView
{
    using Super = CView;
private:
	CPrefabsView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPrefabsView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

public:
	HRESULT	PlaceObstacle(const wstring& strObjectTag, const Matrix& matWorld);

private:
	void	Input();
	void	PlaceObject(const LAYERTAG& eLayerTag, const wstring& strPrototypeTag, const Vec3& vPickPosition, _bool bNotify = true);

private:
	void	InfoView();
	void	TapGroups();

private:
	CTerrain*			m_pTerrainBuffer = nullptr;

	Vec3				m_vPickedPosition;
	LAYERTAG			m_ePickedLayerTag = LAYERTAG::LAYER_END;
	wstring				m_strPickedObject;

	_int				m_Item_Current = 0;

public:
	static class CPrefabsView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTerrain* m_pTerrainGrid);
	virtual void Free() override;
};

END