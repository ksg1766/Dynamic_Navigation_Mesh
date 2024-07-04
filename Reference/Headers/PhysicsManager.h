#pragma once
#include "Base.h"

BEGIN(Engine)

typedef struct PhysicsDesc
{

	_bool UseGravity;
	_bool IsCollided;
	_bool IsKinematic;
}PHYSDESC;

class CPhysicsManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CPhysicsManager);

private:
	CPhysicsManager();
	virtual ~CPhysicsManager() = default;

public:
	void	Tick_Physics(const _float& fTimeDelta);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);

private:

private:
	//vector<>

public:
	virtual void Free() override;
};

END