#include "CollisionManager.h"
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

IMPLEMENT_SINGLETON(CCollisionManager);

CCollisionManager::CCollisionManager()
{
	Reset();
}

void CCollisionManager::LateTick_Collision(const _float& fTimeDelta)
{
	if (CObjectManager::GetInstance()->GetCurrentLevelLayers().empty())	return;

	_uint iOffset = (_uint)LAYERTAG::DEFAULT_LAYER_END;

	::ZeroMemory(m_arrSorted, ((_uint)LAYERTAG::LAYER_END - iOffset) * sizeof(_bool));

	for (_uint iRow = 0; iRow < (_uint)LAYERTAG::LAYER_END - iOffset; ++iRow)
	{
		for (_uint iCol = iRow; iCol < (_uint)LAYERTAG::LAYER_END - iOffset; ++iCol)
		{
			if (m_arrCheck[iRow] & (1 << iCol))
			{
				_uint _iRow = iRow + iOffset;
				_uint _iCol = iCol + iOffset;
				CheckDynamicCollision(reinterpret_cast<LAYERTAG&>(_iRow), reinterpret_cast<LAYERTAG&>(_iCol), fTimeDelta);
				//CheckStaticCollision(reinterpret_cast<LAYERTAG&>(iRow), fTimeDelta);
			}
		}
	}
}

HRESULT CCollisionManager::Reserve_Manager(_uint iNumLevels)
{
	Reset();

	CheckGroup(LAYERTAG::PLAYER, LAYERTAG::UNIT_GROUND);
	CheckGroup(LAYERTAG::PLAYER, LAYERTAG::UNIT_AIR);
	CheckGroup(LAYERTAG::UNIT_GROUND, LAYERTAG::EQUIPMENT);
	//CheckGroup(LAYERTAG::DEFAULT, LAYERTAG::DEFAULT);

	return S_OK;
}

void CCollisionManager::CheckGroup(LAYERTAG eLeft, LAYERTAG eRight)
{
	_uint iOffset = (_uint)LAYERTAG::DEFAULT_LAYER_END;

	_uint iRow = (_uint)eLeft - iOffset;
	_uint iCol = (_uint)eRight - iOffset;

	if (iCol < iRow)
	{
		iRow = (_uint)eRight;
		iCol = (_uint)eLeft;
	}

	if (m_arrCheck[iRow] & (1 << iCol))
	{
		m_arrCheck[iRow] &= ~(1 << iCol);
	}
	else
	{
		m_arrCheck[iRow] |= (1 << iCol);
	}
}

void CCollisionManager::Reset()
{
	::ZeroMemory(m_arrCheck, sizeof(_uint) * (_uint)LAYERTAG::LAYER_END);
}

bool CCollisionManager::IsCollided(CCollider* pLeft, CCollider* pRight)
{
	if (pLeft->Intersects(pRight))
	{
		//if (pLeft->GetRigidBody()->GetBoxCollider()->Intersects(pRight->GetRigidBody()->GetBoxCollider()))
		if (pLeft->GetRigidBody()->GetOBBCollider()->Intersects(pRight->GetRigidBody()->GetOBBCollider()))
			return true;
	}
    return false;
}

void CCollisionManager::CheckDynamicCollision(LAYERTAG& eLayerLeft, LAYERTAG& eLayerRight , const _float& fTimeDelta)
{
	if (eLayerLeft == LAYERTAG::IGNORECOLLISION || eLayerRight == LAYERTAG::IGNORECOLLISION)
		return;

	map<LAYERTAG, class CLayer*>& mapLayers = CObjectManager::GetInstance()->GetCurrentLevelLayers();

	auto iterL = mapLayers.find(eLayerLeft);
	if (mapLayers.end() == iterL)
		return;

	auto iterR = mapLayers.find(eLayerRight);
	if (mapLayers.end() == iterR)
		return;

	vector<CGameObject*>& vecLeft = iterL->second->GetGameObjects();
	vector<CGameObject*>& vecRight = iterR->second->GetGameObjects();
	
	if (!m_arrSorted[(_uint)eLayerRight])
	{	// sweep and prune
		::sort(vecRight.begin(), vecRight.end(), [&](CGameObject* pObjL, CGameObject* pObjR) ->_bool
			{
				CRigidBody* pRigidL = pObjL->GetRigidBody();
				if (!pRigidL) return false;
				CRigidBody* pRigidR = pObjR->GetRigidBody();
				if (!pRigidR) return false;

				BoundingSphere& tBoundingSphereL = pRigidL->GetSphereCollider()->GetBoundingSphere();
				BoundingSphere& tBoundingSphereR = pRigidR->GetSphereCollider()->GetBoundingSphere();
				return tBoundingSphereL.Center.z - tBoundingSphereL.Radius < tBoundingSphereR.Center.z - tBoundingSphereR.Radius;
			});

		m_arrSorted[(_uint)eLayerRight] = true;
	}

	CollisionHash::iterator iter;

	for (auto& iterL : vecLeft)
	{
		CRigidBody* pRigidBodyL = iterL->GetRigidBody();
		if (!pRigidBodyL) continue;

		// 일단 DynamicRigid는 무조건 SphereCollider를 가지고 있다는 전제.
		CSphereCollider* pLeftCol = pRigidBodyL->GetSphereCollider();
		if (nullptr == pLeftCol)
			continue;

		_float	fLeftMaxZ = pLeftCol->GetBoundingSphere().Center.z + pLeftCol->GetBoundingSphere().Radius;
		_bool	bIgnoreRest = false;

		for (auto& iterR : vecRight)
		{
			CRigidBody* pRigidBodyR = iterR->GetRigidBody();
			if (!pRigidBodyR) continue;

			CSphereCollider* pRightCol = pRigidBodyR->GetSphereCollider();
			if (nullptr == pRightCol || iterL == iterR)
				continue;

			if (!bIgnoreRest)
			{
				_float fRightMinZ = pRightCol->GetBoundingSphere().Center.z - pRightCol->GetBoundingSphere().Radius;

				if (false == CompareMaxMinZ(fLeftMaxZ, fRightMinZ)) // false면 이후 무시 or Exit호출
				{
					bIgnoreRest = true;
				}

				COLLIDER_ID ID;
				ID.Left_id = pLeftCol->GetID();
				ID.Right_id = pRightCol->GetID();

				iter = m_hashColInfo.find(ID.ID);

				if (m_hashColInfo.end() == iter)
				{
					m_hashColInfo.emplace(ID.ID, false);
					iter = m_hashColInfo.find(ID.ID);
				}

				if (IsCollided(pLeftCol, pRightCol))
				{	// 현재 충돌 중

					// TODO : bIgnoreRest 가 true일 때는 어떻게 해야 하는가. Exit의 매개변수에 desc가 필요한가...
					COLLISION_DESC lDesc, rDesc;
					MakeCollisionDesc(lDesc, rDesc, pRigidBodyL, pRigidBodyR, fTimeDelta);

					if (iter->second)
					{	// 이전에도 충돌
						if (iterL->IsDead() || iterR->IsDead())
						{	// 둘 중 하나 삭제 예정이면 충돌 해제
							iterL->OnCollisionExit(iterR);
							iterR->OnCollisionExit(iterL);
							pRigidBodyL->OnCollisionExit(lDesc);
							pRigidBodyR->OnCollisionExit(rDesc);
							iter->second = false;
						}
						else
						{
							iterL->OnCollisionStay(iterR);
							iterR->OnCollisionStay(iterL);
							pRigidBodyL->OnCollisionStay(lDesc);
							pRigidBodyR->OnCollisionStay(rDesc);
						}
					}
					else
					{	// 이전에는 충돌 x	// 근데 둘 중 하나 삭제 예정이면 충돌하지 않은 것으로 취급
						if (!iterL->IsDead() && !iterR->IsDead())
						{
							iterL->OnCollisionEnter(iterR);
							iterR->OnCollisionEnter(iterL);
							pRigidBodyL->OnCollisionEnter(lDesc);
							pRigidBodyR->OnCollisionEnter(rDesc);
							iter->second = true;
						}
						else
						{
							iterL->OnCollisionExit(iterR);
							iterR->OnCollisionExit(iterL);
							pRigidBodyL->OnCollisionExit(lDesc);
							pRigidBodyR->OnCollisionExit(rDesc);
							iter->second = false;
						}
					}
				}
				else	// 현재 충돌 x면
				{	
					if (iter->second)	//이전 프레임 충돌
					{	
						// TODO: 아래 desc는 나중에 필요에 따라 없애는거 고려해보도록 하자.
						COLLISION_DESC lDesc, rDesc;
						MakeCollisionDesc(lDesc, rDesc, pRigidBodyL, pRigidBodyR, fTimeDelta);
						//
						iterL->OnCollisionExit(iterR);
						iterR->OnCollisionExit(iterL);
						pRigidBodyL->OnCollisionExit(lDesc);
						pRigidBodyR->OnCollisionExit(rDesc);
						iter->second = false;
					}
				}
			}
			else	// Z비교 결과 무시해도 된다면
			{
				if (iter->second)	// 이전에는 충돌하고 있었다면
				{
					// TODO: 아래 desc는 나중에 필요에 따라 없애는거 고려해보도록 하자.
					COLLISION_DESC lDesc, rDesc;
					MakeCollisionDesc(lDesc, rDesc, pRigidBodyL, pRigidBodyR, fTimeDelta);
					//
					iterL->OnCollisionExit(iterR);
					iterR->OnCollisionExit(iterL);
					pRigidBodyL->OnCollisionExit(lDesc);
					pRigidBodyR->OnCollisionExit(rDesc);
					iter->second = false;
				}
			}
		}
	}
}

void CCollisionManager::CheckStaticCollision(LAYERTAG& eDynamicLayer, const _float& fTimeDelta)
{
	const map<LAYERTAG, class CLayer*>& mapLayers = CObjectManager::GetInstance()->GetCurrentLevelLayers();

	const auto& iterL = mapLayers.find(eDynamicLayer);
	if (mapLayers.end() == iterL)
		return;

	vector<CGameObject*>& vecLeft = iterL->second->GetGameObjects();
	
	CQuadTree* pQuadTreeInstance = CQuadTree::GetInstance();
	
	CollisionHash::iterator iter;

	for (auto& iterL : vecLeft)
	{
		CRigidBody* pRigidBodyL = iterL->GetRigidBody();

		CSphereCollider* pLeftCol = pRigidBodyL->GetSphereCollider();
		if (nullptr == pLeftCol)
			continue;

		CQuadTreeNode* pCurrentNode = pQuadTreeInstance->GetCurrentNodeByPos(iterL->GetTransform()->GetPosition(), pQuadTreeInstance->GetCQuadTreeRoot());
		
		if (!pCurrentNode) return;

		vector<CGameObject*>& vecRight = pCurrentNode->GetObjectList();

		//const _bool	IsLeftTrigger = pLeftCol->IsTrigger();

		for (auto& iterR : vecRight)
		{
			CRigidBody* pRigidBodyR = iterR->GetRigidBody();

			CSphereCollider* pRightCol = pRigidBodyR->GetSphereCollider();
			if (nullptr == pRightCol || iterL == iterR)
				continue;

			COLLIDER_ID ID;
			ID.Left_id = pLeftCol->GetID();
			ID.Right_id = pRightCol->GetID();

			iter = m_hashColInfo.find(ID.ID);

			if (m_hashColInfo.end() == iter)
			{
				m_hashColInfo.insert(make_pair(ID.ID, false));
				iter = m_hashColInfo.find(ID.ID);
			}

			// TODO: 아래 처럼 하든지 TriggerLayer 따로 두고 함수로 트리거 충돌만 따로 검출하든지 선택하자.
			/*const _bool	IsRightTrigger = pRightCol->IsTrigger();

			if (!IsLeftTrigger && IsRightTrigger)
			{
				// ~~~
			}
			else if (IsLeftTrigger && !IsRightTrigger)
			{
				// ~~~
			}
			else if (IsLeftTrigger && IsRightTrigger)
			{
				continue;
			}*/

			if (IsCollided(pLeftCol, pRightCol))
			{	// 현재 충돌 중
				COLLISION_DESC lDesc;
				MakeCollisionDescStatic(lDesc, pRigidBodyL, pRigidBodyR, fTimeDelta);

				if (iter->second)
				{	// 이전에도 충돌
					if (iterL->IsDead() || iterR->IsDead())
					{	// 둘 중 하나 삭제 예정이면 충돌 해제
						pRigidBodyL->OnCollisionExit(lDesc);
						//pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;
					}
					else
					{
						pRigidBodyL->OnCollisionStay(lDesc);
						//pRightCol->OnCollisionStay(pLeftCol);
					}
				}
				else
				{	// 이전에는 충돌 x	// 근데 둘 중 하나 삭제 예정이면 충돌하지 않은 것으로 취급
					if (!iterL->IsDead() && !iterR->IsDead())
					{
						pRigidBodyL->OnCollisionEnter(lDesc);
						//pRightCol->OnCollisionEnter(pLeftCol);
						iter->second = true;
					}
					else
					{
						pRigidBodyL->OnCollisionExit(lDesc);
						//pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;
					}
				}
			}
			else
			{		// 현재 충돌 x면
				if (iter->second)
				{	//이전 프레임 충돌
					COLLISION_DESC lDesc;
					MakeCollisionDescStatic(lDesc, pRigidBodyL, pRigidBodyR, fTimeDelta);

					pRigidBodyL->OnCollisionExit(lDesc);
					//pRightCol->OnCollisionExit(pLeftCol);
					iter->second = false;
				}
			}
		}
	}
}

_bool CCollisionManager::CompareMaxMinZ(_float& fLeftMaxZ, _float& fRightMinZ)
{
	if (fLeftMaxZ < fRightMinZ)
		return false;
	return true;
}

void CCollisionManager::MakeCollisionDesc(OUT COLLISION_DESC& descLeft, OUT COLLISION_DESC& descRight, CRigidBody* lRigid, CRigidBody* rRigid, const _float& fTimeDelta)
{
	Vec3 vVelocityL = dynamic_cast<CRigidDynamic*>(lRigid)->GetLinearVelocity();
	Vec3 vVelocityR = dynamic_cast<CRigidDynamic*>(rRigid)->GetLinearVelocity();

	_float fMassRatio = dynamic_cast<CRigidDynamic*>(lRigid)->GetMass() / dynamic_cast<CRigidDynamic*>(rRigid)->GetMass();	// L/R

	descLeft.pOther = rRigid;
	descLeft.vResultVelocity = (2.f * vVelocityR - (1.f - fMassRatio) * vVelocityL) / (1.f + fMassRatio);
	descLeft.fTimeDelta = fTimeDelta;

	descRight.pOther = lRigid;
	descRight.vResultVelocity = (2.f * vVelocityL - (1.f - 1.f / fMassRatio) * vVelocityR) / (1.f + 1.f / fMassRatio);
	descRight.fTimeDelta = fTimeDelta;
}

void CCollisionManager::MakeCollisionDescStatic(OUT COLLISION_DESC& descLeft, CRigidBody* lRigid, CRigidBody* rRigid, const _float& fTimeDelta)
{
	descLeft.pOther = rRigid;
	descLeft.vResultVelocity = -dynamic_cast<CRigidDynamic*>(lRigid)->GetLinearVelocity();
}

void CCollisionManager::Free()
{
	__super::Free();
}
