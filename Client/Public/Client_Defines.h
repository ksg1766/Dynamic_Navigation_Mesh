#pragma once

#include <process.h>

namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_GAMETOOL, LEVEL_LOADING, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1440;
	const unsigned int		g_iWinSizeY = 810;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace Client;