#pragma once
#include "Engine_Defines.h"
#include "NSHelper.h"

BEGIN(Engine)

class CGameObject;
struct ENGINE_DLL Obst
{
	Vec3 vInnerPoint = Vec3::Zero;
	BoundingBox tAABB;
	CGameObject* pGameObject = nullptr;
	vector<Vec3> vecPoints;

	_bool isDead = false;

	explicit Obst() = default;
	explicit Obst(const Obst& rhs) = default;
	Obst(const Obst& rhs, const Matrix& matWorld, CGameObject* pGameObject) : pGameObject(pGameObject)
	{
		vInnerPoint = Vec3::Transform(rhs.vInnerPoint, matWorld);
		tAABB.Center = Vec3::Transform(rhs.tAABB.Center, matWorld);
		tAABB.Extents = rhs.tAABB.Extents;

		vecPoints.reserve(rhs.vecPoints.size());

		for (_int i = 0; i < rhs.vecPoints.size(); ++i)
		{
			vecPoints.emplace_back(Vec3::Transform(rhs.vecPoints[i], matWorld));
		}

		TRI_REAL fMaxX = -FLT_MAX, fMinX = FLT_MAX, fMaxZ = -FLT_MAX, fMinZ = FLT_MAX;
		for (auto vPoint : vecPoints)
		{
			if (fMaxX < vPoint.x) fMaxX = vPoint.x;
			if (fMinX > vPoint.x) fMinX = vPoint.x;

			if (fMaxZ < vPoint.z) fMaxZ = vPoint.z;
			if (fMinZ > vPoint.z) fMinZ = vPoint.z;
		}

		const _float fAABBOffset = 0.05f;
		tAABB.Center = Vec3((fMaxX + fMinX) * 0.5f, 0.0f, (fMaxZ + fMinZ) * 0.5f);
		tAABB.Extents = Vec3((fMaxX - fMinX) * 0.5f + fAABBOffset, 10.f, (fMaxZ - fMinZ) * 0.5f + fAABBOffset);
	}

	_bool IsOut(const Vec3& vPoint)
	{
		_int iSize = vecPoints.size();

		_int iCrosses = 0;

		for (_int m = 0; m < iSize; ++m)
		{
			_float fSourX = vecPoints[m % iSize].x;
			_float fSourZ = vecPoints[m % iSize].z;

			_float fDestX = vecPoints[(m + 1) % iSize].x;
			_float fDestZ = vecPoints[(m + 1) % iSize].z;

			if ((fSourX > vPoint.x) != (fDestX > vPoint.x))	// x ÁÂÇ¥ °Ë»ç
			{
				_float fAtZ = (fDestZ - fSourZ) * (vPoint.x - fSourX) / (fDestX - fSourX) + fSourZ;

				if (vPoint.z < fAtZ)	// z ÁÂÇ¥ °Ë»ç
				{
					++iCrosses;
				}
			}
		}

		if (0 < iCrosses % 2)
		{
			return false;
		}

		return true;
	}

	Vec3 GetClosestPoint(const Vec3& vPoint, const _float fOffset)
	{
		_float fMinDistanceSq = FLT_MAX;
		Vec3 vMinProjection = Vec3::Zero;
		Vec3 vMinDistance = Vec3::Zero;
		Vec3 vClosestPoint = vPoint;

		Vec3 vSour = Vec3::Zero;
		Vec3 vDest = Vec3::Zero;

		_int iSize = vecPoints.size();

		for (_int m = 0; m < iSize; ++m)
		{
			Vec3 vProjection = CNSHelper::ProjectionPoint2Edge(vPoint, vecPoints[m], vecPoints[(m + 1) % iSize]);
			Vec3 vDistance = vProjection - vPoint;

			_float fDistanceSq = vDistance.LengthSquared();
			if (fDistanceSq < fMinDistanceSq)
			{
				fMinDistanceSq = fDistanceSq;
				vMinProjection = vProjection;
				vMinDistance = vDistance;
			}
		}

		vMinDistance.Normalize();
		return vMinProjection + fOffset * vMinDistance;
	}
};

END