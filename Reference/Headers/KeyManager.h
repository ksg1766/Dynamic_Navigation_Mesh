
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

struct tagKeyInfo
{
	KEYSTATE	eState;
	bool		bPrevPushed;
};

class ENGINE_DLL CKeyManager final : public CBase
{
	DECLARE_SINGLETON(CKeyManager);

public:
	CKeyManager();
	virtual ~CKeyManager() = default;

public:
	HRESULT Reserve_Manager(HWND hWnd);
	void	Tick(_float fTimeDelta);

public:
	KEYSTATE GetKeyState(KEY _eKey) { return m_vecKey[(int)_eKey].eState; }
	const POINT& GetMousePos() { return m_tMousePos; }

private:
	vector<tagKeyInfo> m_vecKey;

	HWND	m_hWnd = nullptr;

	POINT m_tMousePos;
public:
	virtual void Free() override;
};

END
