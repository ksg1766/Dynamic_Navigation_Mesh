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
	_bool	IsOut(Vec3 vPoint, OUT CellData* pNeighbor);
	Vec3	GetPassedEdgeNormal(Vec3 vPoint);

	// cache
	_bool	isDead = false;
	_bool	isNew = false;
};

END