#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)


END

BEGIN(Client)

struct Obst
{
	Vec3 vInnerPoint = Vec3::Zero;
	BoundingBox tAABB;

	vector<Vec3> vecPoints;

	explicit Obst() = default;
	explicit Obst(const Obst& rhs) = default;
	Obst(const Obst& rhs, const Matrix& matWorld)
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
};

END