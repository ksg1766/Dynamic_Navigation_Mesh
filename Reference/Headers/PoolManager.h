#pragma once

#include "Base.h"
#include "Hasher.h"

BEGIN(Engine)

class CGameObject;
class CPoolManager final : public CBase
{
	DECLARE_SINGLETON(CPoolManager)

private:
	CPoolManager();
	virtual ~CPoolManager() = default;

public:
	HRESULT	Reserve_Pool(const wstring& strObjectName, const _uint& iReserveCount, void* pArg = nullptr);

	void	Spawn_Object(const wstring& strObjectName, const Vec3& vSpawnPos);
	void	Restore_Object(CGameObject* pGameObject);

private:
	using POOLS = unordered_map<const wstring, queue<CGameObject*>, djb2Hasher>;
	POOLS m_hashPools;

public:
	virtual void Free() override;
};

END