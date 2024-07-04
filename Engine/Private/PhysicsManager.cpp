#include "PhysicsManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Layer.h"
#include "RigidDynamic.h"
#include "Transform.h"
#include "ColliderSphere.h"
#include "ColliderOBB.h"
#include "QuadTree.h"
#include "QuadTreeNode.h"

IMPLEMENT_SINGLETON(CPhysicsManager);

CPhysicsManager::CPhysicsManager()
{
}

void CPhysicsManager::Tick_Physics(const _float& fTimeDelta)
{

}

HRESULT CPhysicsManager::Reserve_Manager(_uint iNumLevels)
{
	return S_OK;
}

void CPhysicsManager::Free()
{
	Super::Free();
}
