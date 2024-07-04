#pragma once
#include "Base.h"
#include "Hasher.h"

BEGIN(Engine)

class CCollider;
class CSphereCollider;

union COLLIDER_ID
{
	struct
	{
		_uint Left_id;
		_uint Right_id;
	};
	uint64 ID;
};

class CCollisionManager final : public CBase
{
    DECLARE_SINGLETON(CCollisionManager);

private:
	CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	void	LateTick_Collision(const _float& fTimeDelta);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	//map<uint64, _bool>& GetMapColInfo() { return m_mapColInfo; }
	//void SetMapColInfo(ULONGLONG ID, _bool bCollision) { m_mapColInfo[ID] = bCollision; }
	
private:
	void	CheckGroup(LAYERTAG eLeft, LAYERTAG eRight);
	void	Reset();
	
	_bool	IsCollided(CCollider* pLeft, CCollider* pRight);

	void	CheckDynamicCollision(LAYERTAG& eObjectLeft, LAYERTAG& eObjectRight, const _float& fTimeDelta);
	void	CheckStaticCollision(LAYERTAG& eDynamicLayer, const _float& fTimeDelta);
	_bool	CompareMaxMinZ(_float& fLeftMaxZ, _float& fRightMinZ);

	void	MakeCollisionDesc(OUT COLLISION_DESC& descLeft, OUT COLLISION_DESC& descRight, CRigidBody* lRigid, CRigidBody* rRigid, const _float& fTimeDelta);
	void	MakeCollisionDescStatic(OUT COLLISION_DESC& descLeft, CRigidBody* lRigid, CRigidBody* rRigid, const _float& fTimeDelta);

private:
	using CollisionHash = unordered_map<uint64, _bool, MurmurHash3Hasher>;
	CollisionHash m_hashColInfo;

	_uint	m_arrCheck[(_uint)LAYERTAG::LAYER_END - (_uint)LAYERTAG::DEFAULT_LAYER_END] = { 0 };
	_bool	m_arrSorted[(_uint)LAYERTAG::LAYER_END - (_uint)LAYERTAG::DEFAULT_LAYER_END] = { 0 };

public:
	virtual void Free() override;
};

END