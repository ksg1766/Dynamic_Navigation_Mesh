#include "../Public/TimerManager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimerManager)

CTimerManager::CTimerManager()
{
}

_float CTimerManager::Compute_TimeDelta(const wstring & strTimerTag)
{
	CTimer*	pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Compute_TimeDelta();	
}

HRESULT CTimerManager::Add_Timer(const wstring& strTimerTag)
{
	CTimer*	pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	m_Timers.insert({ strTimerTag, CTimer::Create() });

	return S_OK;
}

CTimer * CTimerManager::Find_Timer(const wstring& strTimerTag) const
{
	auto iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimerManager::Free(void)
{
	__super::Free();

	for (auto& Pair : m_Timers)
	{
		Safe_Release(Pair.second);
	}
	m_Timers.clear();


}