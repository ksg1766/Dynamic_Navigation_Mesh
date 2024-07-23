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

_bool CellData::IsOut(Vec3 vPoint, OUT CellData*& pNeighbor)
{
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

	return false;
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
