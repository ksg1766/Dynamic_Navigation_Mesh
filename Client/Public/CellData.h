#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)


END

BEGIN(Client)

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
	_bool	IsOut(Vec3 vPoint, OUT CellData*& pNeighbor);
	Vec3	GetPassedEdgeNormal(Vec3 vPoint);
	inline Vec3	GetCenter()	{ return (vPoints[0] + vPoints[1] + vPoints[2]) / 3.f; }

	inline static _float CostBetween(CellData* pSour, CellData* pDest)	{ return Vec3::Distance(pSour->GetCenter(), pDest->GetCenter()); }
	//inline static _float CostBetween(CellData* pSour, CellData* pDest)	{ return fabs(pSour->GetCenter().x - pDest->GetCenter().x) + fabs(pSour->GetCenter().z - pDest->GetCenter().z); }
	inline static _float HeuristicCost(CellData* pSour, CellData* pDest){ return CostBetween(pSour, pDest); }
	inline static _float HeuristicCost(CellData* pSour, Vec3 vDest)		{ return Vec3::Distance(pSour->GetCenter(), vDest); }
	//inline static _float HeuristicCost(CellData* pSour, Vec3 vDest)		{ return fabs(pSour->GetCenter().x - vDest.x) + fabs(pSour->GetCenter().z - vDest.z); }

	// cache
	_bool	isDead = false;
	_bool	isNew = false;
};

END