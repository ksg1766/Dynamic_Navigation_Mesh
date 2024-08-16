#include "NSHelper.h"

BEGIN(Engine)

_float CNSHelper::PerpendicularDistance(const Vec3& vPt, const Vec3& vLineStart, const Vec3& vLineEnd)
{
	_float fDx = vLineEnd.x - vLineStart.x;
	_float fDz = vLineEnd.z - vLineStart.z;

	// Normalize
	_float fMag = pow(pow(fDx, 2.0f) + pow(fDz, 2.0f), 0.5f);
	if (fMag > 0.0f)
	{
		fDx /= fMag; fDz /= fMag;
	}

	_float fPvx = vPt.x - vLineStart.x;
	_float fPvz = vPt.z - vLineStart.z;

	// Get dot product (project fPv onto normalized direction)
	_float fPvdot = fDx * fPvx + fDz * fPvz;

	// Scale line direction vector
	_float fDsx = fPvdot * fDx;
	_float fDsz = fPvdot * fDz;

	// Subtract this from fPv
	_float fAx = fPvx - fDsx;
	_float fAz = fPvz - fDsz;

	return pow(pow(fAx, 2.0f) + pow(fAz, 2.0f), 0.5f);
}

HRESULT CNSHelper::RamerDouglasPeucker(const vector<Vec3>& vecPoints, _float fEpsilon, vector<Vec3>& vecOut)
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
		// Just return start and end points
		vecOut.clear();
		vecOut.push_back(vecPoints[0]);
		vecOut.push_back(vecPoints[iEnd]);
	}

	return S_OK;
}

_float CNSHelper::TriArea2x(const Vec3& vP0, const Vec3& vP1, const Vec3& vP2)
{
	const _float fAx = vP1.x - vP0.x;
	const _float fAz = vP1.z - vP0.z;
	const _float fBx = vP2.x - vP0.x;
	const _float fBz = vP2.z - vP0.z;

	return fBx * fAz - fAx * fBz;
}

Vec3 CNSHelper::ProjectionPoint2Edge(const Vec3& vPoint, const Vec3& vP1, const Vec3& vP2)
{
	Vec3 vToLine = vP2 - vP1;
	Vec3 vToPoint = vPoint - vP1;

	_float fT = vToPoint.Dot(vToLine) / vToLine.LengthSquared();
	if (fT < 0.f) fT = 0.f;
	if (fT > 1.f) fT = 1.f;

	return vP1 + fT * vToLine;
}

_float CNSHelper::DistanceEdge2Edge(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2)
{	// 참고 : https://wizardmania.tistory.com/21
	Vec3 U = vP2 - vP1;
	Vec3 V = vQ2 - vQ1;
	Vec3 W = vP1 - vQ1;
	_float fA = U.Dot(U);
	_float fB = U.Dot(V);
	_float fC = V.Dot(V);
	_float fD = U.Dot(W);
	_float fE = V.Dot(W);
	_float fDenom = fA * fC - fB * fB;
	_float fSc, fSNum, fSDenom = fDenom;
	_float fTc, fTNum, fTDenom = fDenom;

	if (fDenom < FLT_EPSILON)	// 선분이 평행한 경우
	{
		fSNum = 0.0f;
		fSDenom = 1.0f;
		fTNum = fE;
		fTDenom = fC;
	}
	else
	{	// 선분이 평행하지 않은 경우
		fSNum = (fB * fE - fC * fD);
		fTNum = (fA * fE - fB * fD);

		if (fSNum < 0.0f)	// SNum < 0인 경우, s = 0
		{
			fSNum = 0.0f;
			fTNum = fE;
			fTDenom = fC;
		}
		else if (fSNum > fSDenom)	// SNum > SDenom인 경우, s = 1
		{
			fSNum = fSDenom;
			fTNum = fE + fB;
			fTDenom = fC;
		}
	}

	if (fTNum < 0.0f)		// TNum < 0인 경우, t = 0
	{
		fTNum = 0.0f;
		if (-fD < 0.0f)
		{
			fSNum = 0.0f;
		}
		else if (-fD > fA)
		{
			fSNum = fSDenom;
		}
		else
		{
			fSNum = -fD;
			fSDenom = fA;
		}
	}
	else if (fTNum > fTDenom)	// TNum > TDenom인 경우, t = 1
	{
		fTNum = fTDenom;
		if ((-fD + fB) < 0.0f)
		{
			fSNum = 0.0f;
		}
		else if ((-fD + fB) > fA)
		{
			fSNum = fSDenom;
		}
		else
		{
			fSNum = (-fD + fB);
			fSDenom = fA;
		}
	}

	// Sc와 Tc를 최종적으로 계산
	fSc = (fabs(fSNum) < FLT_EPSILON) ? 0.0f : fSNum / fSDenom;
	fTc = (fabs(fTNum) < FLT_EPSILON) ? 0.0f : fTNum / fTDenom;

	// 두 선분의 가장 가까운 점 사이의 벡터를 계산
	Vec3 vDP = W + (U * fSc) - (V * fTc);

	return vDP.Length();
}

END