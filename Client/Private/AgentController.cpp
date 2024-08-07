#include "stdafx.h"
#include "AgentController.h"
#include "GameInstance.h"
#include "Agent.h"
#include "Obstacle.h"
#include "Terrain.h"
#include "DebugDraw.h"

constexpr auto EPSILON = 0.001f;

CAgentController::CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(100.0f, 100.0f, 100.0f))
	, m_vMaxLinearSpeed(Vec3(200.0f, 200.0f, 200.0f))
	, m_fAgentRadius(3.3f)
{
}

CAgentController::CAgentController(const CAgentController& rhs)
	:Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_vMaxLinearSpeed(rhs.m_vMaxLinearSpeed)
	, m_fAgentRadius(rhs.m_fAgentRadius)
{
}

HRESULT CAgentController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAgentController::Initialize(void* pArg)
{
	m_pTransform = GetTransform();
	m_pTransform->SetScale(m_fAgentRadius * 2.0f * Vec3::One);

	if (nullptr != pArg)
	{
		CAgent::AgentDesc* pDesc = reinterpret_cast<CAgent::AgentDesc*>(pArg);
		m_pCurrentCell = pDesc->pStartCell;
		m_pCells = pDesc->pCells;
		m_pCellGrids = pDesc->pCellGrids;
		m_pObstGrids = pDesc->pObstGrids;
	}

#pragma region DebugDraw
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t		iLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
		return E_FAIL;
	}
#pragma endregion DebugDraw

	m_vLinearSpeed = m_vMaxLinearSpeed;

#pragma region AStarPerformance
	/*if (FAILED(m_pGameInstance->Add_Timer(TEXT("Timer_AStar"))))
		return E_FAIL;*/
#pragma endregion AStarPerformance

	return S_OK;
}

void CAgentController::Tick(_float fTimeDelta)
{
	if (true == IsMoving() || false == IsOutOfWorld())
	{
		Slide(Move(fTimeDelta));
	} PopPath();
}

void CAgentController::LateTick(_float fTimeDelta)
{
	m_vPrePos = m_pTransform->GetPosition();
}

_bool CAgentController::IsIdle()
{
	return !IsMoving();
}

_bool CAgentController::IsMoving()
{
	return m_isMoving;
}

_bool CAgentController::IsOutOfWorld()
{
	const Vec3& vPosition = m_pTransform->GetPosition();

	if (vPosition.x >= 0.5f * gWorldCX || vPosition.x <= -0.5f * gWorldCX || vPosition.z >= 0.5f * gWorldCZ || vPosition.z <= -0.5f * gWorldCZ)
		return true;

	return false;
}

void CAgentController::ForceHeight()
{
	m_pTransform->Translate(Vec3(0.f, GetHeightOffset(), 0.f));
}

_float CAgentController::GetHeightOffset()
{
	Vec3 vPos(m_pTransform->GetPosition());

	const array<Vec3, POINT_END>* vPoints = &m_pCurrentCell->vPoints;

	Plane vPlane = Plane(
		m_pCurrentCell->vPoints[POINT_A],
		m_pCurrentCell->vPoints[POINT_B],
		m_pCurrentCell->vPoints[POINT_C]
	);

	return -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y + vPos.y);
}

Vec3 CAgentController::Move(_float fTimeDelta)
{
	Vec3 vDistance = Vec3::Zero;
	Vec3 vMoveAmount = Vec3::Zero;
	Vec3 vDirection = Vec3::Zero;

	Vec3 vPrePos = m_pTransform->GetPosition();

	if (1 >= m_dqWayPoints.size()) // already in destcell or no pathfinding
	{
		vDistance = m_vDestPos - vPrePos;
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
		{	// end
			vMoveAmount = vDistance;

			m_dqPortals.clear();
			m_dqOffset.clear();
			m_dqPath.clear();
			m_dqWayPoints.clear();
			m_dqExpandedVertices.clear();

			m_isMoving = false;
		}
	}
	else
	{
		vDistance = m_dqWayPoints[1] - vPrePos;
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
		{	// to next waypoint
			if (false == m_dqWayPoints.empty())
			{
				m_dqWayPoints.pop_front();
			}
		}
	}
	
	m_pTransform->Translate(vMoveAmount);

	return vPrePos;
}

void CAgentController::Slide(const Vec3 vPrePos)
{
	CellData* pNeighbor = nullptr;
	Vec3 vPosition = m_pTransform->GetPosition();

	if (false == m_pCurrentCell->IsOut(vPosition, pNeighbor))
	{
		return;
	}

	while (nullptr != pNeighbor)
	{
		if (false == pNeighbor->IsOut(vPosition, pNeighbor))
		{
			m_pCurrentCell = pNeighbor;
			return;
		}
	}

	Vec3 vDir = vPosition - vPrePos;
	Vec3 vPassedLine = m_pCurrentCell->GetPassedEdgeNormal(vPosition);

	vPosition = vPrePos + vDir - (EPSILON + vDir.Dot(vPassedLine)) * vPassedLine;
	m_pCurrentCell = FindCellByPosition(vPosition);

	while (nullptr == m_pCurrentCell)
	{
		Obst* pObst = FindObstByPosition(vPosition);
		if (nullptr != pObst)
		{
			vPosition = pObst->GetClosestPoint(vPosition, m_fAgentRadius + EPSILON);
			m_pCurrentCell = FindCellByPosition(vPosition);
		}
	}

	m_pTransform->SetPosition(vPosition);
}

_bool CAgentController::AStar()
{
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> Open;
	unordered_map<CellData*, PATH> Path;
	unordered_set<CellData*> Closed;

	m_dqPath.clear();
	m_dqPortals.clear();
	m_dqOffset.clear();
	m_dqExpandedVertices.clear();
	m_dqWayPoints.clear();

	// start node
	Vec3 vStartPos = m_pTransform->GetPosition();

	Vec3 vDistance = m_vDestPos - vStartPos;
	_float h = vDistance.Length();

	Open.push(PQNode{ h, 0.0f, m_pCurrentCell });
	Path[m_pCurrentCell] = PATH(m_pCurrentCell, LINE_END);

	while (false == Open.empty())
	{
		PQNode tNode = Open.top();
		CellData* pCurrent = tNode.pCell;

		if (pCurrent == m_pDestCell)
		{
			pair<CellData*, LINES>& tNext = Path[m_pDestCell];
			auto& [pNext, ePassed] = tNext;

			m_dqPortals.push_front(pair(
				m_vDestPos,
				m_vDestPos
			));

			m_dqExpandedVertices.push_front(m_dqPortals.front());
			
			//while (m_pCurrentCell != pNext)
			while (true)
			{
				m_dqPath.push_front(tNext);

				m_dqPortals.push_front(pair(
					pNext->vPoints[ePassed],
					pNext->vPoints[(ePassed + 1) % POINT_END]
				));

				m_dqExpandedVertices.push_front(m_dqPortals.front());

				if (m_pCurrentCell == pNext)
					break;

				tNext = Path[pNext];
			}

			const Vec3& vStartPos = m_pTransform->GetPosition();

			m_dqPortals.push_front(pair(
				vStartPos,
				vStartPos
			));

			m_dqExpandedVertices.push_front(m_dqPortals.front());

			return true;
		}

		Open.pop();
		Closed.emplace(pCurrent);

		for (uint8 i = LINE_AB; i < LINE_END; ++i)
		{
			CellData* pNeighbor = pCurrent->pNeighbors[i];	// parent의 인접셀이 nullptr이 아니라면
			if (nullptr == pNeighbor)
			{
				continue;
			}

			// portal length
			// 4.1 Width Calculation
			auto& [pParent, ePassedLine] = Path[pCurrent];

			LINES eLine1 = LINE_END;

			if (LINE_END != ePassedLine)
			{				
				for (uint8 j = LINE_AB; j < LINE_END; ++j)
				{
					if (pCurrent->pNeighbors[j] == pParent)
					{
						eLine1 = (LINES)j;
						break;
					}
				}

				_float fHalfWidth = pCurrent->fHalfWidths[POINTS((5 - eLine1 - i) % 3)];
				if (fHalfWidth < m_fAgentRadius)
				{
					continue;
				}
			}
			else if ((pCurrent->vPoints[(i + 1) % POINT_END] - pCurrent->vPoints[i]).LengthSquared() < powf(2.0f * m_fAgentRadius, 2.0f))
			{
				continue;
			}

			_float neighbor_g = 0.0f;

			Vec3 vNextEdgeDir = pCurrent->vPoints[(i + 1) % POINT_END] - pCurrent->vPoints[i];
			vNextEdgeDir.Normalize();

			if (m_pCurrentCell == pCurrent)
			{
				neighbor_g = tNode.g + CellData::CostBetweenPoint2Edge(
					vStartPos,
					pCurrent->vPoints[i] + m_fAgentRadius * vNextEdgeDir,
					pCurrent->vPoints[(i + 1) % POINT_END] - m_fAgentRadius * vNextEdgeDir
				);
			}
			else
			{
				/*Vec3 vCurrEdgeDir = pParent->vPoints[(ePassedLine + 1) % POINT_END] - pParent->vPoints[ePassedLine];
				vCurrEdgeDir.Normalize();

				neighbor_g = tNode.g + CellData::CostBetweenEdge2Edge(
					pParent->vPoints[ePassedLine] + m_fAgentRadius * vCurrEdgeDir,
					pParent->vPoints[(ePassedLine + 1) % POINT_END] - m_fAgentRadius * vCurrEdgeDir,
					pCurrent->vPoints[i] + m_fAgentRadius * vNextEdgeDir,
					pCurrent->vPoints[(i + 1) % POINT_END] - m_fAgentRadius * vNextEdgeDir
				);*/
				
				//Vec3 vCurrEdgeDir = pParent->vPoints[(ePassedLine + 1) % POINT_END] - pParent->vPoints[ePassedLine];
				//vCurrEdgeDir.Normalize();

				//_float fTheta = acosf(vCurrEdgeDir.Dot(vNextEdgeDir));
				
				/*_float fTheta = pCurrent->fTheta[((POINTS)eLine1 == (POINTS)i) ? (POINTS)eLine1 : (POINTS)((eLine1 + 1) % POINT_END)];

				neighbor_g = tNode.g + m_fAgentRadius * fTheta;*/

				/*neighbor_g = CellData::CostBetweenMax(
					pParent->vPoints[ePassedLine],
					pParent->vPoints[(ePassedLine + 1) % POINT_END],
					pCurrent->vPoints[i],
					pCurrent->vPoints[(i + 1) % POINT_END],
					vStartPos,
					m_vDestPos,
					tNode.g,
					tNode.f - tNode.g,
					m_fAgentRadius
				);*/

				neighbor_g = pCurrent->CostBetweenMax(
					(POINTS)eLine1,
					(POINTS)((eLine1 + 1) % POINT_END),
					(POINTS)i,
					(POINTS)((i + 1) % POINT_END),
					vStartPos,
					m_vDestPos,
					tNode.g,
					tNode.f - tNode.g,
					m_fAgentRadius
				);
			}

			if (Closed.end() == Closed.find(pNeighbor))
			{
				Path[pNeighbor] = PATH(pCurrent, (LINES)i);	// pCurrent의 i번째 line을 통과한 노드가 pNeighbor

				Vec3 vEdgeMid = 0.5f * (pCurrent->vPoints[i] + pCurrent->vPoints[(i + 1) % POINT_END]);
				Open.push(PQNode {
					neighbor_g + CellData::HeuristicCostEuclidean(vEdgeMid, m_vDestPos),
					neighbor_g,
					pNeighbor
				});
			}
		}
	}

	return false;
}

void CAgentController::FunnelAlgorithm()
{
	//deque<pair<Vec3, Vec3>> dqOffset;
	m_dqOffset.emplace_back(Vec3::Zero, Vec3::Zero);

	for (_int i = 2; i < m_dqPortals.size(); ++i)
	{
		const auto& [vLeft0, vRight0] = m_dqPortals[i - 2];
		const auto& [vLeft1, vRight1] = m_dqPortals[i - 1];
		const auto& [vLeft2, vRight2] = m_dqPortals[i];

		Vec3 vLineL0 = vLeft1 - vLeft0;
		Vec3 vLineL1 = vLeft2 - vLeft1;

		Vec3 vLineR0 = vRight1 - vRight0;
		Vec3 vLineR1 = vRight2 - vRight1;

		if (Vec3::Zero == vLineL0)
		{
			if (Vec3::Zero == vLineL1)
			{
				//vLineL0 = m_dqOffset.back().first;
				//vLineL0.Normalize();
				//vLineL1 = vLineL0;
			}
			else
			{
				vLineL1.Normalize();
				//vLineL0 = vLineL1;
			}			
		}
		else
		{
			if (Vec3::Zero == vLineL1)
			{
				vLineL0.Normalize();
				//vLineL1 = vLineL0;
			}
			else
			{
				vLineL0.Normalize();
				vLineL1.Normalize();
			}
		}

		if (Vec3::Zero == vLineR0)
		{
			if (Vec3::Zero == vLineR1)
			{
				//vLineR0 = m_dqOffset.back().second;
				//vLineR0.Normalize();
				//vLineR1 = vLineR0;
			}
			else
			{
				vLineR1.Normalize();
				//vLineR0 = vLineR1;
			}
		}
		else
		{
			if (Vec3::Zero == vLineR1)
			{
				vLineR0.Normalize();
				//vLineR1 = vLineR0;
			}
			else
			{
				vLineR0.Normalize();
				vLineR1.Normalize();
			}
		}

		Vec3 vAvgL = vLineL0 + vLineL1;
		Vec3 vAvgR = vLineR0 + vLineR1;

		Vec3 vPerpendL = Vec3::Zero;
		Vec3 vPerpendR = Vec3::Zero;

		if (Vec3::Zero != vAvgL)
		{
			vAvgL.Normalize();
			vPerpendL = Vec3(m_fAgentRadius * vAvgL.z, vAvgL.y, m_fAgentRadius * -vAvgL.x);
		}
		else
		{
			vPerpendL = m_dqOffset.back().first;
		}

		if (Vec3::Zero != vAvgR)
		{
			vAvgR.Normalize();
			vPerpendR = Vec3(m_fAgentRadius * -vAvgR.z, vAvgR.y, m_fAgentRadius * vAvgR.x);
		}
		else
		{
			vPerpendR = m_dqOffset.back().second;
		}

		m_dqOffset.emplace_back(vPerpendL, vPerpendR);
	}

	m_dqOffset.emplace_back(Vec3::Zero, Vec3::Zero);

	//
	Vec3 vPortalApex = m_pTransform->GetPosition();		// 초기 상태
	Vec3 vPortalLeft = m_pTransform->GetPosition();
	Vec3 vPortalRight = m_pTransform->GetPosition();

	_int iApexIndex = 0;
	_int iLeftIndex = 0;
	_int iRightIndex = 0;

	m_dqWayPoints.push_back(vPortalApex);

	for (_int i = 1; i < m_dqOffset.size(); ++i)
	{
		const auto& [vLeft, vRight] = m_dqPortals[i];

		//Vec3 vLeft = vOriginL;
		//Vec3 vRight = vOriginR;

		if (TriArea2x(vPortalApex, vPortalRight, vRight) <= 0.0f)
		{
			if (vPortalApex == vPortalRight || TriArea2x(vPortalApex, vPortalLeft, vRight) > 0.0f)
			{
				vPortalRight = vRight;				// funnel 당기기
				iRightIndex = i;
			}
			else
			{
				//if (Vec3::Zero != m_dqOffset[iLeftIndex].first)
					m_dqWayPoints.push_back(vPortalLeft + m_dqOffset[iLeftIndex].first);	// Right가 Left를 넘었다면 Left를 waypoint에 추가				

				vPortalApex = vPortalLeft;				// L을 새로운 시작점으로
				iApexIndex = iLeftIndex;

				vPortalRight = vPortalApex;				// 초기화
				iRightIndex = iApexIndex;

				i = iApexIndex;							// 재시작 인덱스
				continue;
			}
		}

		if (TriArea2x(vPortalApex, vPortalLeft, vLeft) >= 0.0f)
		{
			if (vPortalApex == vPortalLeft || TriArea2x(vPortalApex, vPortalRight, vLeft) < 0.0f)
			{
				vPortalLeft = vLeft;
				iLeftIndex = i;
			}
			else
			{
				//if (Vec3::Zero != m_dqOffset[iRightIndex].second)
					m_dqWayPoints.push_back(vPortalRight + m_dqOffset[iRightIndex].second);

				vPortalApex = vPortalRight;
				iApexIndex = iRightIndex;

				vPortalLeft = vPortalApex;
				iLeftIndex = iApexIndex;

				i = iApexIndex;
				continue;
			}
		}
	}

	m_dqWayPoints.push_back(m_vDestPos);
}

CellData* CAgentController::FindCellByPosition(const Vec3& vPosition)
{
	CellData* pCell = nullptr;

	_int iX = (vPosition.x + gWorldCX * 0.5f) / gGridCX;
	_int iZ = (vPosition.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iKey = iZ * gGridX + iX;

	auto [begin, end] = m_pCellGrids->equal_range(iKey);
	
	for (auto cell = begin; cell != end; ++cell)
	{
		if (false == cell->second->IsOut(vPosition, pCell))
		{
			return cell->second;
		}
	}

	return nullptr;
}

Obst* CAgentController::FindObstByPosition(const Vec3& vPosition)
{
	_int iX = (vPosition.x + gWorldCX * 0.5f) / gGridCX;
	_int iZ = (vPosition.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iKey = iZ * gGridX + iX;

	auto [begin, end] = m_pObstGrids->equal_range(iKey);

	for (auto obst = begin; obst != end; ++obst)
	{
		if (false == obst->second->IsOut(vPosition))
		{
			return obst->second;
		}
	}

	return nullptr;
}

_bool CAgentController::Pick(CTerrain* pTerrain, _uint screenX, _uint screenY)
{
	_float fDistance = 0.0f;
	Vec3 vPickedPos;
	if (false == pTerrain->Pick(screenX, screenY, vPickedPos, fDistance, pTerrain->GetTransform()->WorldMatrix()))
	{
		return false;
	}

	//m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));

	m_pCurrentCell = FindCellByPosition(m_pTransform->GetPosition());	// TODO: ...
	m_pDestCell = FindCellByPosition(vPickedPos);

	if (nullptr != m_pDestCell)
	{
		m_vDestPos = vPickedPos;

		if (true == AStar())
		{
			FunnelAlgorithm();

			m_isMoving = true;

			/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
			fAStarPerformance = fAStarPerformance;*/
			return true;
		}
	}

	Obst* pObst = FindObstByPosition(vPickedPos);

	if (nullptr != pObst)
	{
		m_vDestPos = pObst->GetClosestPoint(vPickedPos, m_fAgentRadius + 0.1f);
		m_pDestCell = FindCellByPosition(m_vDestPos);

		if (true == AStar())
		{
			FunnelAlgorithm();

			m_isMoving = true;

			/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
			fAStarPerformance = fAStarPerformance;*/
			return true;
		}

		m_vDestPos = m_pTransform->GetPosition();
	}

	/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
	fAStarPerformance = fAStarPerformance;*/
	return false;
}

void CAgentController::Input(_float fTimeDelta)
{
}

void CAgentController::DebugRender()
{
	m_pBatch->Begin();
	if (false == m_dqPath.empty())
	{
		for (_int i = m_dqPath.size() - 1; i >= 0; --i)
		{
			for (uint8 j = LINE_AB; j < LINE_END; ++j)
			{
				const auto& [pNext, ePassed] = m_dqPath[i];

				if (j != ePassed)
				{
					m_pBatch->DrawLine(
						VertexPositionColor(pNext->vPoints[j], Colors::Cyan),
						VertexPositionColor(pNext->vPoints[(j + 1) % POINT_END], Colors::Cyan));
				}
			}
		}
	}

	if (false == m_dqPortals.empty())
	{
		for (_int i = 0; i < m_dqPortals.size(); ++i)
		{
			const auto& [vLeft, vRight] = m_dqPortals[i];
			const auto& [vLeftOff, vRightOff] = m_dqOffset[i];
			const auto& [vLeftP, vRightP] = m_dqExpandedVertices[i];

			m_pBatch->DrawLine(
				VertexPositionColor(vLeft, Colors::Blue),
				VertexPositionColor(vRight, Colors::Blue));

			DX::DrawRing(m_pBatch, vLeftP, m_fAgentRadius * Vec3::UnitX, m_fAgentRadius * Vec3::UnitZ, Colors::Yellow);
			DX::DrawRing(m_pBatch, vRightP, m_fAgentRadius * Vec3::UnitX, m_fAgentRadius * Vec3::UnitZ, Colors::Snow);

			m_pBatch->DrawLine(
				VertexPositionColor(vLeftP, Colors::LightGray),
				VertexPositionColor(vLeftP + vLeftOff, Colors::LightGray));
			m_pBatch->DrawLine(
				VertexPositionColor(vRightP, Colors::LightGray),
				VertexPositionColor(vRightP + vRightOff, Colors::LightGray));
		}
	}

	if (false == m_dqWayPoints.empty())
	{
		for (size_t i = 0; i < m_dqWayPoints.size() - 1; ++i)
		{
			m_pBatch->DrawLine(
				VertexPositionColor(m_dqWayPoints[i], Colors::Coral),
				VertexPositionColor(m_dqWayPoints[i + 1], Colors::Coral));
		}
	}	
	m_pBatch->End();
}

void CAgentController::PopPath()
{
	for (_int i = 0; i < m_dqPath.size(); ++i)
	{
		if (m_pCurrentCell == m_dqPath[i].first)
		{
			for (_int j = 0; j <= i; ++j)
			{
				if (false == m_dqPath.empty())				m_dqPath.pop_front();
				if (false == m_dqPortals.empty())			m_dqPortals.pop_front();
				if (false == m_dqOffset.empty())			m_dqOffset.pop_front();
				if (false == m_dqExpandedVertices.empty())	m_dqExpandedVertices.pop_front();
			}
			break;
		}
	}
}

CAgentController* CAgentController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAgentController* pInstance = new CAgentController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAgentController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAgentController::Clone(CGameObject* pGameObject, void* pArg)
{
	CAgentController* pInstance = new CAgentController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAgentController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAgentController::Free()
{
	// DebugDraw
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	m_dqWayPoints.clear();
	m_dqPortals.clear();
	m_dqOffset.clear();
	m_dqExpandedVertices.clear();
	m_dqPath.clear();

	Super::Free();
}
