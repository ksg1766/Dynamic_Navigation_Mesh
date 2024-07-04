#pragma once

#include "Converter_Defines.h"

class CConverterMain
{
public:
	CConverterMain();
	virtual ~CConverterMain() = default;

public:
	/* 내 게임의 시작. */
	HRESULT Execute();

private:
	HRESULT Convert_All();

	HRESULT Convert_Static(wstring fileName, wstring savePath);
	HRESULT	Convert_Skeletal(wstring fileName, wstring savePath);

private:

};