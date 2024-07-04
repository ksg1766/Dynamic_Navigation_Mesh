#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;	
}

_ulong CBase::Release()
{
	/* ªË¡¶«œ≥Æ. */
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
	{
		/* ∞®º“«œ≥Æ. */
		return m_dwRefCnt--;
	}
}

void CBase::Free()
{
}
