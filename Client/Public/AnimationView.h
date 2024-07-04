#pragma once
#include "View.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CLayer;
class CGameObject;

END

BEGIN(Client)

class CAnimationView final : public CView
{
    using Super = CView;
private:
	CAnimationView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnimationView() = default;

public:
	virtual HRESULT Initialize(void* pArg)	override;
	virtual HRESULT Tick()					override;
	virtual HRESULT LateTick()				override;
	virtual HRESULT	DebugRender()			override;

public:
	CGameObject* SelectedObject() { return m_pCurPickedObject; }

private:
	void		Input();

private:
	void		InfoView();
	void		ItemGroup();
	void		AnimationGroup();
	void		DeleteReservedGroup();

	HRESULT		LoadAnimations();
	HRESULT		ExportAnimations(const wstring& strModelFilePath);

private:
	CGameObject*		m_pAnimModelObject = nullptr;
	string				m_strSelectedObject;
	string				m_strSelectedAnimation;

	CGameObject*		m_pCurPickedObject = nullptr;
	CGameObject*		m_pPrePickedObject = nullptr;

	_int				m_Item_Current = 0;
	_int				m_Animation_Current = 0;
	_int				m_Delete_Current = 0;

	vector<const _char*> m_vecAnimationNames;
	vector<const _char*> m_vecDeleteReserved;

public:
	static class CAnimationView* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END