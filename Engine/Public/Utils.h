#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL Utils
{
public:
	static bool StartsWith(string str, string comp);
	static bool StartsWith(wstring str, wstring comp);

	static void Replace(OUT string& str, string comp, string rep);
	static void Replace(OUT wstring& str, wstring comp, wstring rep);

	static wstring ToWString(string value);
	static string ToString(wstring value);

	static void CreateRandomTexture1DSRV(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV);
};

END