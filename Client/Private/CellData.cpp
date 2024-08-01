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
	POINTS C = POINTS((5 - eLine1 - eLine2) % 3);
	POINTS A = POINTS((C + 1) % 3);
	POINTS B = POINTS((C + 2) % 3);

	_float d = ::min(
		(vPoints[(eLine1 + 1) % 3] - vPoints[eLine1]).Length(),
		(vPoints[(eLine2 + 1) % 3] - vPoints[eLine2]).Length());
	
	LINES c = LINES(3 - eLine1 - eLine2);
	
	if (IsObtuse(C, A, B) || IsObtuse(C, B, A))
		return d;
	else if (nullptr == pNeighbors[c])
		return CostBetweenPoint2Edge(vPoints[C], vPoints[c], vPoints[(c + 1) % 2]);
	else
		return SearchWidth(C, this, c, d);
}

_float CellData::SearchWidth(POINTS C, CellData* T, LINES e, _float d)
{
	POINTS U = POINTS(e);
	POINTS V = POINTS((e + 1) % 3);

	if (IsObtuse(C, U, V) || IsObtuse(C, V, U))
		return d;

	_float _d = CostBetweenPoint2Edge(vPoints[C], vPoints[U], vPoints[V]);

	if (_d > d)
		return d;
	else if (nullptr == pNeighbors[e])
		return _d;
	else
	{
		CellData* _T = T->pNeighbors[e];

		LINES _e1 = LINE_END, _e2 = LINE_END;

		for (uint8 i = 0; i < LINE_END; ++i)
		{
			if (T != _T->pNeighbors[i])
			{
				if (LINE_END == _e1)
					_e1 = (LINES)i;
				else
					_e2 = (LINES)i;
			}
		}		

		d = pNeighbors[e]->SearchWidth(C, _T, _e1, d);
		return pNeighbors[e]->SearchWidth(C, _T, _e2, d);
	}
}
