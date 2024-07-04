#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CGameObject;
class CTexture;

END

BEGIN(Client)

typedef struct DissolveDesc
{
	CGameObject* pDissolveObject = nullptr;
	_float	fPlayTime = 3.f;
	_float	fCurTime = 0.f;
	_bool	IsRunning = false;

}DISSOLVE_DESC;

class CDissolveManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CDissolveManager);
private:
	CDissolveManager();
	virtual ~CDissolveManager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice);
	void	Tick_Dissolve(const _float& fTimeDelta);

public:
	void	AddDissolve(CGameObject* pDissolveObject, _float fPlayTime = 3.f);

private:
	list<DISSOLVE_DESC>	m_listDissolve;

	//_float					m_fDissolveTimer = 0.f;
	ID3D11ShaderResourceView* m_ppDissolveSRV;
	CGameInstance*			m_pGameInstance = nullptr;

public:
	virtual void Free() override;
};

END