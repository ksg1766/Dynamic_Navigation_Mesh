#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ �����ڰ� ������ ����Ŭ�������� �θ� �Ǵ� Ŭ���� .*/
/* �������� Ŭ���̾� �ִ� ������ �����ϰų� ������ �Լ��� ȣ�� �� �� �ְ� �ȴ�. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual HRESULT Tick(const _float& fTimeDelta);
	virtual HRESULT LateTick(const _float& fTimeDelta);
	virtual HRESULT	DebugRender() { return S_OK; };

protected:
	class CGameInstance*		m_pGameInstance;
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END