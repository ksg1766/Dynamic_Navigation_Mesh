#pragma once

#include "Client_Defines.h"

/* Ŭ������ ����� ���. */
/* 
class CŬ�����̸�
{
private or protected:
	������. 
	�Ҹ���.
public: 
	Getter 
public:	 
	Setter 
public:
	�Լ�
protected:
	����
protected:
	�Լ�
private:
	����
private:
	�Լ�

public:
	���������� �Լ�. (Create, Clone)	
	���������� �Լ�. (Free)
};
*/

/* �� ������ �������� �ʱ�ȭ(Initialize), ����(Tick), ������(Render)�� �����Ѵ�. */

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
	/* �� ������ ����. */
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
	/* ��緹������ ���Ǵ� ������Ʈ ����. */
	HRESULT Reserve_Client_Managers();
	HRESULT Ready_Prototype_Components(); 
	HRESULT Ready_Prototype_Scripts(); 

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END

