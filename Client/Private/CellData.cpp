#include "stdafx.h"
#include "CellData.h"

void CellData::CW()
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

void CellData::SetUpNormals()
{
	Vec3 vLines[LINE_END];

	vLines[LINE_AB] = vPoints[POINT_B] - vPoints[POINT_A];
	vLines[LINE_BC] = vPoints[POINT_C] - vPoints[POINT_B];
	vLines[LINE_CA] = vPoints[POINT_A] - vPoints[POINT_C];

	for (size_t i = 0; i < LINE_END; i++)
	{
		vNormals[i] = Vec3(vLines[i].z * -1.f, 0.f, vLines[i].x);
		vNormals[i].Normalize();
	}
}

_bool CellData::ComparePoints(const Vec3& pSour, const Vec3& pDest)
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

_bool CellData::IsOut(const Vec3& vPoint, OUT CellData*& pNeighbor)
{
	_bool bReturn = false;
	for (size_t i = 0; i < LINE_END; i++)
	{
		Vec3 vSour = vPoint - vPoints[i];
		vSour.Normalize();
		Vec3 vDest = vNormals[i];
		vDest.Normalize();

		if (0 < vSour.Dot(vDest))
		{
			pNeighbor = pNeighbors[i];
			return true;
		}
	}

	return bReturn;
}

Vec3 CellData::GetPassedEdgeNormal(Vec3 vPoint)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		Vec3	vSour = vPoint - vPoints[i];
		vSour.Normalize();
		Vec3	vDest = vNormals[i];
		vDest.Normalize();

		if (0 < vSour.Dot(vDest))
		{
			return vNormals[i];
		}
	}

	return Vec3::Zero;
}

_float CellData::CostBetweenMax(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2, const Vec3& vStart, const Vec3& vDest, _float fParentG, _float fParentH)
{
	Vec3 vClosestPoint2Edge = ProjectionPoint2Edge(vStart, vQ1, vQ2);
	_float fCostPoint2Edge = (vClosestPoint2Edge - vStart).Length();
	_float fCostEdge2Edge = DistanceEdge2Edge(vP1, vP2, vQ1, vQ2);

	Vec3 vMidPoint = 0.5f * (vQ1 + vQ2);
	_float fNeighborH = HeuristicCostEuclidean(vMidPoint, vDest);
	_float fCostHeuristicDiff = fParentG + fParentH - fNeighborH;

	return ::max(::max(fCostPoint2Edge, fCostEdge2Edge), fCostHeuristicDiff);
}

_float CellData::CalculateWidth(LINES eLine1, LINES eLine2)
{
	// AB BC -> B	// 0 1 -> 1
	// AB CA -> A	// 0 2 -> 0
	// BC CA -> C	// 1 2 -> 2
	// BC AB -> B	// 1 0 -> 1
	// CA AB -> A	// 2 0 -> 0
	// CA BC -> C	// 2 1 -> 2

	POINTS C = ;

	if (IsObtuse(POINTS((eLine1 + 1) % 3), POINTS((eLine1 + 2) % 3), POINTS((eLine2 + 2) % 3)) ||
		IsObtuse(POINTS((eLine1 + 1) % 3), POINTS((eLine2 + 2) % 3), POINTS((eLine1 + 2) % 3)))


	return _float();
}

_float CellData::SearchWidth(LINES eLine1, LINES eLine2)
{


	return _float();
}
