#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CTransformView : public CView
{
    using Super = CView;
private:
	CTransformView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransformView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

public:
	//void	SetTransform(CTransform* pTransform) { m_pTransform = pTransform; }
	void	SetObject(CGameObject* pGameObject);

private:
	void	Input();
	
private:
	void	InfoView();
	void	TransformController();

private:
	string				m_strPickedObject;
	//CTransform*			m_pTransform;
	CGameObject*		m_pGameObject = nullptr;

	_int				m_Item_Current = 0;

	// Dissolve Test
	_float				m_fDissolveAmount = 0.f;

public:
	static class CTransformView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END