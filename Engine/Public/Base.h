#pragma once

#include "Engine_Defines.h"

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ų��. */
	/* 	unsigned long : ������Ű�� �� ������ ���۷���ī��Ʈ�� �����Ѵ�. */
	_ulong AddRef();
	
	/* ���۷��� ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�.  */
	/* 	unsigned long : ���ҽ�Ű�� �� ������ ���۷���ī��Ʈ�� �����Ѵ�. */
	_ulong Release();

private:
	_ulong		m_dwRefCnt = 0;

public:
	virtual void Free();

};

