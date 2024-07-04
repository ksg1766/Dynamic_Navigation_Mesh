#include "KeyManager.h"

IMPLEMENT_SINGLETON(CKeyManager);

int g_arrVK[(int)KEY::TYPE_END] =
{
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	VK_TAB,
	VK_OEM_4,
	VK_OEM_6,
	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9,
	VK_INSERT, VK_DELETE, VK_HOME, VK_END, VK_PRIOR, VK_NEXT,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'Q','W','E','R','T','Y','U','I','O','P',
	'A','S','D','F','G','H','J','K','L',
	'Z','X','C','V','B','N','M',
	VK_MENU,
	VK_CONTROL,
	VK_LSHIFT,
	VK_SPACE,
	VK_RETURN,
	VK_ESCAPE,

	VK_LBUTTON,
	VK_RBUTTON
};

CKeyManager::CKeyManager()
{
}

HRESULT CKeyManager::Reserve_Manager(HWND hWnd)
{
	m_hWnd = hWnd;

	m_vecKey.reserve(_uint(KEY::TYPE_END));
	for (_uint i = 0; i < (_uint)KEY::TYPE_END; ++i)
		m_vecKey.push_back(tagKeyInfo{ KEYSTATE::NONE, false });

	ZeroMemory(&m_tMousePos, sizeof(POINT));

	return S_OK;
}

void CKeyManager::Tick(_float fTimeDelta)
{
	HWND hWnd = GetFocus();

	if (hWnd != nullptr)
	{
		for (int i = 0; i < (int)KEY::TYPE_END; ++i)
		{
			if (GetAsyncKeyState(g_arrVK[i]) & 0x8000)
			{
				if (m_vecKey[i].bPrevPushed)
					m_vecKey[i].eState = KEYSTATE::HOLD;
				else
					m_vecKey[i].eState = KEYSTATE::TAP;

				m_vecKey[i].bPrevPushed = true;
			}
			else
			{
				if (m_vecKey[i].bPrevPushed)
					m_vecKey[i].eState = KEYSTATE::AWAY;
				else
					m_vecKey[i].eState = KEYSTATE::NONE;

				m_vecKey[i].bPrevPushed = false;
			}
		}
	}

	else
	{
		for (int i = 0; i < (int)KEY::TYPE_END; ++i)
		{
			m_vecKey[i].bPrevPushed = false;

			if (KEYSTATE::TAP == m_vecKey[i].eState || KEYSTATE::HOLD == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEYSTATE::AWAY;
			}
			else if (KEYSTATE::AWAY == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEYSTATE::NONE;
			}
		}
	}

	GetCursorPos(&m_tMousePos);
	ScreenToClient(m_hWnd, &m_tMousePos);
}

void CKeyManager::Free()
{
}
