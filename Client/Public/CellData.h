#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "NSHelper.h"

BEGIN(Engine)


END

BEGIN(Client)

constexpr _int gWorldCX = 1024;
constexpr _int gWorldCZ = 1024;
constexpr _int gGridCX = 64;
constexpr _int gGridCZ = 64;
constexpr _int gGridX = 16;
constexpr _int gGridZ = 16;

enum POINTS : uint8 { POINT_A, POINT_B, POINT_C, POINT_END };
enum LINES : uint8 { LINE_AB, LINE_BC, LINE_CA, LINE_END };

struct CellData
{
	array<Vec3, POINT_END> vPoints = { Vec3::Zero, Vec3::Zero, Vec3::Zero };
	array<CellData*, LINE_END> pNeighbors = { nullptr, nullptr, nullptr };
	array<Vec3, LINE_END> vNormals = { Vec3::Zero, Vec3::Zero, Vec3::Zero };
	
	void	CW();
	void	SetUpNormals();

	_bool	ComparePoints(const Vec3& pSour, const Vec3& pDest);
	_bool	IsOut(const Vec3& vPoint, OUT CellData*& pNeighbor);
	Vec3	GetPassedEdgeNormal(Vec3 vPoint);
	inline Vec3	GetCenter()	{ return (vPoints[0] + vPoints[1] + vPoints[2]) / 3.f; }

	inline static _float CostBetween(CellData* pSour, CellData* pDest)	{ return Vec3::Distance(pSour->GetCenter(), pDest->GetCenter()); }
	inline static _float HeuristicCostEuclidean(CellData* pSour, CellData* pDest)		{ return CostBetween(pSour, pDest); }
	inline static _float HeuristicCostEuclidean(CellData* pSour, const Vec3& vDest)		{ return Vec3::Distance(pSour->GetCenter(), vDest); }
	inline static _float HeuristicCostEuclidean(const Vec3& vSour, const Vec3& vDest)	{ return Vec3::Distance(vSour, vDest); }

	inline static _float CostBetweenPoint2Edge(const Vec3& vStart, const Vec3& vQ1, const Vec3& vQ2)
	{
		Vec3 vClosestPoint2Edge = ProjectionPoint2Edge(vStart, vQ1, vQ2);
		return (vClosestPoint2Edge - vStart).Length();
	}
	
	inline static _float CostBetweenEdge2Edge(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2)
	{
		return DistanceEdge2Edge(vP1, vP2, vQ1, vQ2);
	}
	
	static _float CostBetweenMax(const Vec3& vP1, const Vec3& vP2, const Vec3& vQ1, const Vec3& vQ2, const Vec3& vStart, const Vec3& vDest, _float fParentG, _float fParentH);

	//
	inline _bool IsObtuse(POINTS eP0, POINTS eP1, POINTS eP2)
	{
		return (vPoints[eP0] - vPoints[eP2]).LengthSquared() >= ((vPoints[eP2] - vPoints[eP1]).LengthSquared() + (vPoints[eP1] - vPoints[eP0]).LengthSquared());
	}

	_float CalculateWidth(LINES eLine1, LINES eLine2);
	_float SearchWidth(LINES eLine1, LINES eLine2);

	// cache
	_bool	isDead = false;
	_bool	isNew = false;
};

END