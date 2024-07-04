#pragma once

#include "Base.h"

BEGIN(Engine)

class CInputDevice final : public CBase
{
	DECLARE_SINGLETON(CInputDevice)
private:
	CInputDevice();
	virtual ~CInputDevice() = default;

public:
	HRESULT Ready_InputDevice(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDevice();

public:
	_ubyte	Get_DIKeyState(_ubyte byKeyID)					{ return m_CurKeyState[byKeyID];	}
	_ubyte	Get_DIMouseState(MOUSEKEYSTATE eMouseKeyID)		{ return m_CurMouseState.rgbButtons[eMouseKeyID]; }
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)	{ return ((_long*)&m_CurMouseState)[eMouseMoveID]; }

	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Pressing(_ubyte byKeyID);
	_bool	Key_Up(_ubyte byKeyID);

	_bool	Mouse_Down(MOUSEKEYSTATE eMouseKeyID);
	_bool	Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouseKeyID);

	_bool	Get_AnyKeyDown();

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_ubyte				m_PreKeyState[256] = { 0 };
	_ubyte				m_CurKeyState[256] = { 0 };
	DIMOUSESTATE		m_PreMouseState;
	DIMOUSESTATE		m_CurMouseState;

public:
	virtual void Free();
};

END