#pragma once

#include "Base.h"

/* 사본 객체들을 리스트에 보관한다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject, const LAYERTAG& eLayerTag);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void DebugRender();

	vector<class CGameObject*>& GetGameObjects()		{ return m_GameObjects; }
	LAYERTAG&	GetLayerTag()							{ return m_eLayerTag; }
	void		SetLayerTag(const LAYERTAG& eLayerTag)	{ m_eLayerTag = eLayerTag; }

private:
	LAYERTAG					m_eLayerTag;
	vector<class CGameObject*>	m_GameObjects;
public:
	static CLayer* Create();
	virtual void Free() override;
};

END