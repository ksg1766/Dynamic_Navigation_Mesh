#include "..\Public\InputDevice.h"

IMPLEMENT_SINGLETON(CInputDevice)

CInputDevice::CInputDevice()
{
}

_bool CInputDevice::Key_Down(_ubyte byKeyID)
{
	if (m_PreKeyState[byKeyID] != m_CurKeyState[byKeyID])
	{
		if (m_CurKeyState[byKeyID] & 0x80)
			return true;
	}
	/*for (int i = 0; i < 256; ++i)
	{
		if (m_PreKeyState[i] && !(m_CurKeyState[i] & 0x80))
			m_PreKeyState[i] = !m_PreKeyState[i];
	}*/
	return false;
}

_bool CInputDevice::Key_Up(_ubyte byKeyID)
{
	if (m_PreKeyState[byKeyID] != m_CurKeyState[byKeyID])
	{
		if (m_PreKeyState[byKeyID] & 0x80)
			return true;
	}
	/*for (int i = 0; i < 256; ++i)
	{
		if (!m_PreKeyState[byKeyID] && (m_CurKeyState[byKeyID] & 0x80))
			m_PreKeyState[i] = !m_PreKeyState[i];
	}*/
	return false;
}

_bool CInputDevice::Key_Pressing(_ubyte byKeyID)
{
	if (m_PreKeyState[byKeyID] == m_CurKeyState[byKeyID])
	{
		if (m_PreKeyState[byKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInputDevice::Mouse_Down(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_CurMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_CurMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInputDevice::Mouse_Up(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_CurMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_PreMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInputDevice::Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] == m_CurMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_CurMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

HRESULT CInputDevice::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;
	if (FAILED(m_pKeyBoard->Acquire()))
		return E_FAIL;

	::ZeroMemory(m_PreKeyState, sizeof(m_PreKeyState));

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	::ZeroMemory(&m_PreMouseState, sizeof(m_PreMouseState));

	return S_OK;
}

_bool CInputDevice::Get_AnyKeyDown()
{
	if (Key_Down(DIK_1)
		|| Key_Down(DIK_2)
		|| Key_Down(DIK_3)
		|| Key_Down(DIK_4)
		|| Key_Down(DIK_5)
		|| Key_Down(DIK_6)
		|| Key_Down(DIK_7)
		|| Key_Down(DIK_8)
		|| Key_Down(DIK_9)
		|| Key_Down(DIK_0)
		|| Key_Down(DIK_MINUS)
		|| Key_Down(DIK_EQUALS)
		|| Key_Down(DIK_BACK)
		|| Key_Down(DIK_TAB)
		|| Key_Down(DIK_Q)
		|| Key_Down(DIK_W)
		|| Key_Down(DIK_E)
		|| Key_Down(DIK_R)
		|| Key_Down(DIK_T)
		|| Key_Down(DIK_Y)
		|| Key_Down(DIK_U)
		|| Key_Down(DIK_I)
		|| Key_Down(DIK_O)
		|| Key_Down(DIK_P)
		|| Key_Down(DIK_LBRACKET)
		|| Key_Down(DIK_RBRACKET)
		|| Key_Down(DIK_RETURN)
		|| Key_Down(DIK_LCONTROL)
		|| Key_Down(DIK_A)
		|| Key_Down(DIK_S)
		|| Key_Down(DIK_D)
		|| Key_Down(DIK_F)
		|| Key_Down(DIK_G)
		|| Key_Down(DIK_H)
		|| Key_Down(DIK_J)
		|| Key_Down(DIK_K)
		|| Key_Down(DIK_L)
		|| Key_Down(DIK_SEMICOLON)
		|| Key_Down(DIK_APOSTROPHE)
		|| Key_Down(DIK_GRAVE)
		|| Key_Down(DIK_LSHIFT)
		|| Key_Down(DIK_BACKSLASH)
		|| Key_Down(DIK_Z)
		|| Key_Down(DIK_X)
		|| Key_Down(DIK_C)
		|| Key_Down(DIK_V)
		|| Key_Down(DIK_B)
		|| Key_Down(DIK_N)
		|| Key_Down(DIK_M)
		|| Key_Down(DIK_COMMA)
		|| Key_Down(DIK_PERIOD)
		|| Key_Down(DIK_SLASH)
		|| Key_Down(DIK_RSHIFT)
		|| Key_Down(DIK_MULTIPLY)
		|| Key_Down(DIK_LMENU)
		|| Key_Down(DIK_SPACE)
		|| Key_Down(DIK_CAPITAL)
		|| Key_Down(DIK_F1)
		|| Key_Down(DIK_F2)
		|| Key_Down(DIK_F3)
		|| Key_Down(DIK_F4)
		|| Key_Down(DIK_F5)
		|| Key_Down(DIK_F6)
		|| Key_Down(DIK_F7)
		|| Key_Down(DIK_F8)
		|| Key_Down(DIK_F9)
		|| Key_Down(DIK_F10)
		|| Key_Down(DIK_NUMLOCK)
		|| Key_Down(DIK_SCROLL)
		|| Key_Down(DIK_NUMPAD7)
		|| Key_Down(DIK_NUMPAD8)
		|| Key_Down(DIK_NUMPAD9)
		|| Key_Down(DIK_SUBTRACT)
		|| Key_Down(DIK_NUMPAD4)
		|| Key_Down(DIK_NUMPAD5)
		|| Key_Down(DIK_NUMPAD6)
		|| Key_Down(DIK_ADD)
		|| Key_Down(DIK_NUMPAD1)
		|| Key_Down(DIK_NUMPAD2)
		|| Key_Down(DIK_NUMPAD3)
		|| Key_Down(DIK_NUMPAD0)
		|| Key_Down(DIK_DECIMAL)
		|| Key_Down(DIK_OEM_102)
		|| Key_Down(DIK_F11)
		|| Key_Down(DIK_F12)
		|| Key_Down(DIK_F13)
		|| Key_Down(DIK_F14)
		|| Key_Down(DIK_F15)
		|| Key_Down(DIK_KANA)
		|| Key_Down(DIK_ABNT_C1)
		|| Key_Down(DIK_CONVERT)
		|| Key_Down(DIK_NOCONVERT)
		|| Key_Down(DIK_YEN)
		|| Key_Down(DIK_ABNT_C2)
		|| Key_Down(DIK_NUMPADEQUALS)
		|| Key_Down(DIK_PREVTRACK)
		|| Key_Down(DIK_AT)
		|| Key_Down(DIK_COLON)
		|| Key_Down(DIK_UNDERLINE)
		|| Key_Down(DIK_KANJI)
		|| Key_Down(DIK_STOP)
		|| Key_Down(DIK_AX)
		|| Key_Down(DIK_UNLABELED)
		|| Key_Down(DIK_NEXTTRACK)
		|| Key_Down(DIK_NUMPADENTER)
		|| Key_Down(DIK_RCONTROL)
		|| Key_Down(DIK_MUTE)
		|| Key_Down(DIK_CALCULATOR)
		|| Key_Down(DIK_PLAYPAUSE)
		|| Key_Down(DIK_MEDIASTOP)
		|| Key_Down(DIK_VOLUMEDOWN)
		|| Key_Down(DIK_VOLUMEUP)
		|| Key_Down(DIK_WEBHOME)
		|| Key_Down(DIK_NUMPADCOMMA)
		|| Key_Down(DIK_DIVIDE))
	{
		return true;
	}

	return false;
}

void CInputDevice::Update_InputDevice()
{
	::CopyMemory(&m_PreKeyState, &m_CurKeyState, sizeof(_char) * 256);
	m_pKeyBoard->GetDeviceState(256, m_CurKeyState);
	::CopyMemory(&m_PreMouseState, &m_CurMouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_CurMouseState);
}

void CInputDevice::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);

	Safe_Release(m_pInputSDK);
}
