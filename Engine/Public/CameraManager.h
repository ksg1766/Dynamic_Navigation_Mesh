#pragma once
#include "Base.h"
#include "Hasher.h"

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
	void	UpdateReflectionMatrix(_float fWaterLevel);
	_matrix GetReflectionMatrix()		{ return m_matReflect; }

	CGameObject* GetCurrentCamera()		{ return m_pCurrentCamera; }

private:
	using CameraHash = unordered_map<const wstring, CGameObject*, djb2Hasher>;
	CameraHash		m_hashCamera;
	CGameObject*	m_pCurrentCamera = nullptr;
	_matrix			m_matReflect;

public:
	virtual void Free() override;
};

END