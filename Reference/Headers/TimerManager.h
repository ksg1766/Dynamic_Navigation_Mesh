#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimerManager : public CBase
{
	DECLARE_SINGLETON(CTimerManager)

private:
	CTimerManager();
	virtual ~CTimerManager() = default;

public:
	_float Compute_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT	Add_Timer(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*>		m_Timers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag) const;

public:
	virtual void	Free(void);
};

END