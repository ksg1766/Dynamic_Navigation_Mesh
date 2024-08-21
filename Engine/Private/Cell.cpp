#include "Cell.h"
#include "DebugDraw.h"

void Cell::CW()
{
	Vec3 vA(vPoints[POINT_A].x, 0.f, vPoints[POINT_A].z);
	Vec3 vB(vPoints[POINT_B].x, 0.f, vPoints[POINT_B].z);
	Vec3 vC(vPoints[POINT_C].x, 0.f, vPoints[POINT_C].z);

	Vec3 vFlatAB = vB - vA;
	Vec3 vFlatBC = vC - vB;
	Vec3 vResult;
	vFlatAB.Cross(vFlatBC, vResult);

	if (vResult.y < 0.f)
	{
		::swap(vPoints[POINT_B], vPoints[POINT_C]);
	}
}

void Cell::SetUpData()
{
	Vec3 vLines[LINE_END];

	vLines[LINE_AB] = vPoints[POINT_B] - vPoints[POINT_A];
	vLines[LINE_BC] = vPoints[POINT_C] - vPoints[POINT_B];
	vLines[LINE_CA] = vPoints[POINT_A] - vPoints[POINT_C];

	for (uint8 i = 0; i < LINE_END; i++)
	{
		vLines[i].Normalize();
	}

	for (uint8 i = 0; i < LINE_END; i++)
	{
		fTheta[i] = acosf(vLines[i].Dot(-vLines[(i + 2) % 3]));
	}
}

_bool Cell::ComparePoints(const Vec3& pSour, const Vec3& pDest)
{
	if (pSour == vPoints[POINT_A])
	{
		if (pDest == vPoints[POINT_B])
			return true;

		if (pDest == vPoints[POINT_C])
			return true;
	}

	if (pSour == vPoints[POINT_B])
	{
		if (pDest == vPoints[POINT_A])
			return true;

		if (pDest == vPoints[POINT_C])
			return true;
	}

	if (pSour == vPoints[POINT_C])
	{
		if (pDest == vPoints[POINT_A])
			return true;

		if (pDest == vPoints[POINT_B])
			return true;
	}

	return false;
}

_bool Cell::IsOut(const Vec3& vPoint, OUT Cell*& pNeighbor)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		if (CNSHelper::TriArea2x(vPoints[i], vPoint, vPoints[(i + 1) % POINT_END]) > 0.0f)
		{
			pNeighbor = pNeighbors[i];
			return true;
		}
	}

	return false;
}

Vec3 Cell::GetPassedEdgeNormal(Vec3 vPoint)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		if (CNSHelper::TriArea2x(vPoints[i], vPoint, vPoints[(i + 1) % POINT_END]) > 0.0f)
		{
			Vec3 vNormal = Vec3(vPoints[i].z - vPoints[(i + 1) % POINT_END].z, 0.f, vPoints[(i + 1) % POINT_END].x - vPoints[i].x);
			vNormal.Normalize();
			return vNormal;
		}
	}

	return Vec3::Zero;
}

_float Cell::CostBetweenMax(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2, const Vec3& vStart, const Vec3& vDest, _float fParentG, _float fParentH, _float fAgentRadius)
{
	Vec3 vClosestPoint2Edge = CNSHelper::ProjectionPoint2Edge(vStart, vQ1, vQ2);
	_float fCostPoint2Edge = (vClosestPoint2Edge - vStart).Length();

	Vec3 vEdge1 = vP2 - vP1;
	Vec3 vEdge2 = vQ2 - vQ1;
	vEdge1.Normalize();
	vEdge2.Normalize();
	_float fTheta = acosf(vEdge1.Dot(vEdge2));
	_float fCostEdge2Edge = fParentG + fAgentRadius * fTheta;

	Vec3 vMidPoint = 0.5f * (vQ1 + vQ2);
	_float fNeighborH = HeuristicCostEuclidean(vMidPoint, vDest);
	_float fCostHeuristicDiff = fParentG + fParentH - fNeighborH;

	return ::max(::max(fCostPoint2Edge, fCostEdge2Edge), fCostHeuristicDiff);
}

_float Cell::CostBetweenMax(POINTS eP1, POINTS eP2, POINTS eQ1, POINTS eQ2, const Vec3& vStart, const Vec3& vDest, _float fParentG, _float fParentH, _float fAgentRadius)
{
	Vec3 vClosestPoint2Edge = CNSHelper::ProjectionPoint2Edge(vStart, vPoints[eQ1], vPoints[eQ2]);
	_float fCostPoint2Edge = (vClosestPoint2Edge - vStart).Length();

	POINTS eBetween = POINT_END;
	(eP1 == eQ1) ? eBetween = eP1 : eBetween = eP2;

	_float fCostEdge2Edge = fParentG + fAgentRadius * fTheta[eBetween];

	_float fNeighborH = HeuristicCostEuclidean(vClosestPoint2Edge, vDest);
	_float fCostHeuristicDiff = fParentG + fParentH - fNeighborH;

	return ::max(::max(fCostPoint2Edge, fCostEdge2Edge), fCostHeuristicDiff);
}

_float Cell::CalculateHalfWidth(LINES eLine1, LINES eLine2)
{
	if (eLine1 == eLine2)
		return -FLT_MAX;

	POINTS C = POINTS((5 - eLine1 - eLine2) % 3);
	POINTS A = POINTS((C + 1) % 3);
	POINTS B = POINTS((C + 2) % 3);

	LINES c = LINES(3 - eLine1 - eLine2);

	_float d = ::min(
		(vPoints[c] - vPoints[C]).Length(),
		(vPoints[(c + 1) % 3] - vPoints[C]).Length());

	if (IsObtuse(vPoints[C], vPoints[A], vPoints[B]) || IsObtuse(vPoints[C], vPoints[B], vPoints[A]))
		return 0.5f * d;
	else if (nullptr == pNeighbors[c])
		return 0.5f * CostBetweenPoint2Edge(vPoints[C], vPoints[A], vPoints[B]);
	else
		return 0.5f * SearchWidth(vPoints[C], this, c, d);
}

_float Cell::SearchWidth(const Vec3& C, Cell* T, LINES e, _float d)
{
	Vec3 U = T->vPoints[e];
	Vec3 V = T->vPoints[(e + 1) % 3];

	V.y = U.y = C.y;

	if (IsObtuse(C, U, V) || IsObtuse(C, V, U))
		return d;

	_float _d = CostBetweenPoint2Edge(C, U, V);

	if (_d > d)
		return d;
	else if (nullptr == T->pNeighbors[e])
		return _d;
	else
	{
		Cell* _T = T->pNeighbors[e];

		LINES _e1 = LINE_END, _e2 = LINE_END;

		for (uint8 i = 0; i < LINE_END; ++i)
		{
			if (T != _T->pNeighbors[i])
				(LINE_END == _e1) ? _e1 = (LINES)i : _e2 = (LINES)i;
		}

		d = SearchWidth(C, _T, _e1, d);
		return SearchWidth(C, _T, _e2, d);
	}
}

_bool Cell::Pick(const Ray& ray, OUT Vec3& pickPos, OUT _float& distance, const Matrix& matWorld)
{
	if (ray.Intersects(vPoints[POINT_A], vPoints[POINT_B], vPoints[POINT_C], OUT distance))
	{
		if (isnan(distance))
			return false;

		pickPos = ray.position + ray.direction * distance;
		return true;
	}

	return false;
}
