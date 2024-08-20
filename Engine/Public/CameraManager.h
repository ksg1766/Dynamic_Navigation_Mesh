#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CCameraManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CCameraManager)
private:
	CCameraManager();
	virtual ~CCameraManager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);

	void Tick(const _float& fTimeDelta);
	void LateTick(const _float& fTimeDelta);
	void DebugRender();

public:
	HRESULT AddCamera(const wstring& strName, CGameObject* pCamera);
	HRESULT DeleteCamera(const wstring& strName);
	HRESULT ChangeCamera(const wstring& strName);
	HRESULT ChangeCamera();
	void	UpdateReflectionMatrix(_float fWaterLevel);
	_matrix GetReflectionMatrix()		{ return m_matReflect; }

	CGameObject* GetCurrentCamera()		{ return m_pCurrentCamera; }

private:
	map<const wstring, CGameObject*> m_mapCamera;
	CGameObject*	m_pCurrentCamera = nullptr;
	_matrix			m_matReflect;

public:
	virtual void Free() override;
};

END