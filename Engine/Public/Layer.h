#pragma once

#include "Base.h"

/* �纻 ��ü���� ����Ʈ�� �����Ѵ�. */

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