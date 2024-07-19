#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

_float PerpendicularDistance(const Vec3& vPt, const Vec3& vLineStart, const Vec3& vLineEnd)
{
	_float fDx = vLineEnd.x - vLineStart.x;
	_float fDz = vLineEnd.z - vLineStart.z;

	//Normalize
	_float fMag = pow(pow(fDx, 2.0f) + pow(fDz, 2.0f), 0.5f);
	if (fMag > 0.0f)
	{
		fDx /= fMag; fDz /= fMag;
	}

	_float fPvx = vPt.x - vLineStart.x;
	_float fPvz = vPt.z - vLineStart.z;

	//Get dot product (project fPv onto normalized direction)
	_float fPvdot = fDx * fPvx + fDz * fPvz;

	//Scale line direction vector
	_float fDsx = fPvdot * fDx;
	_float fDsz = fPvdot * fDz;

	//Subtract this from fPv
	_float fAx = fPvx - fDsx;
	_float fAz = fPvz - fDsz;

	return pow(pow(fAx, 2.0f) + pow(fAz, 2.0f), 0.5f);
}

HRESULT RamerDouglasPeucker(const vector<Vec3>& vecPoints, _float fEpsilon, vector<Vec3>& vecOut)
{
	if (vecPoints.size() < 2)
	{
		MSG_BOX("Not enough points to simplify");
		return E_FAIL;
	}

	// Find the point with the maximum distance from line between start and end
	_float fDmax = 0.0f;
	size_t iIndex = 0;
	size_t iEnd = vecPoints.size() - 1;
	for (size_t i = 1; i < iEnd; i++)
	{
		_float fD = PerpendicularDistance(vecPoints[i], vecPoints[0], vecPoints[iEnd]);
		if (fD > fDmax)
		{
			iIndex = i;
			fDmax = fD;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if (fDmax > fEpsilon)
	{
		// Recursive call
		vector<Vec3> vecRecResults1;
		vector<Vec3> vecRecResults2;
		vector<Vec3> vecFirstLine(vecPoints.begin(), vecPoints.begin() + iIndex + 1);
		vector<Vec3> vecLastLine(vecPoints.begin() + iIndex, vecPoints.end());
		RamerDouglasPeucker(vecFirstLine, fEpsilon, vecRecResults1);
		RamerDouglasPeucker(vecLastLine, fEpsilon, vecRecResults2);

		// Build the result list
		vecOut.assign(vecRecResults1.begin(), vecRecResults1.end() - 1);
		vecOut.insert(vecOut.end(), vecRecResults2.begin(), vecRecResults2.end());
		if (vecOut.size() < 2)
		{
			MSG_BOX("Problem assembling output");
			return E_FAIL;
		}
	}
	else
	{
		//Just return start and end points
		vecOut.clear();
		vecOut.push_back(vecPoints[0]);
		vecOut.push_back(vecPoints[iEnd]);
	}

	return S_OK;
}

END