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
	/* �����ϳ�. */
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
	{
		/* �����ϳ�. */
		return m_dwRefCnt--;
	}
}

void CBase::Free()
{
}
