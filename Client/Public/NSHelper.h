#pragma once
#include "Base.h"
#include "Client_Defines.h"

_float PerpendicularDistance(const Vec3& vPt, const Vec3& vLineStart, const Vec3& vLineEnd);

HRESULT RamerDouglasPeucker(const vector<Vec3>& vecPoints, _float fEpsilon, vector<Vec3>& vecOut);

_float TriArea2x(const Vec3& vP0, const Vec3& vP1, const Vec3& vP2);

Vec3 ProjectionPoint2Edge(const Vec3& vPoint, const Vec3& vP1, const Vec3& vP2);

_float DistanceEdge2Edge(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2);