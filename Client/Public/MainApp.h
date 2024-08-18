#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(const _float& fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CRenderer*				m_pRenderer = { nullptr };

	_uint					m_iFps = 0;
	DWORD					m_dwTime = 0;
	_tchar					m_szFPS[64];

private:
	HRESULT Open_Level(LEVELID eLevelID);
	HRESULT Reserve_Client_Managers();
	HRESULT Ready_Prototype_Components(); 
	HRESULT Ready_Prototype_Scripts(); 

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END