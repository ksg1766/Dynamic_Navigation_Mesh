#include "NavMeshAgent.h"
#include "ComponentManager.h"
#include "GameInstance.h"
#include "LevelManager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "FileUtils.h"
#include "Cell.h"
#include "Obstacle.h"
#include "GameObject.h"
#include "LevelManager.h"
#include "DebugDraw.h"

unordered_multimap<_int, Cell*>* CNavMeshAgent::m_pCellGrids;
unordered_multimap<_int, Obst*>* CNavMeshAgent::m_pObstGrids;

constexpr auto EPSILON = 0.0001f;

CNavMeshAgent::CNavMeshAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, ComponentType::NavMeshAgent)
	, m_vLinearSpeed(Vec3(100.0f, 100.0f, 100.0f))
	, m_vMaxLinearSpeed(Vec3(200.0f, 200.0f, 200.0f))
	, m_fAgentRadius(3.4f)
	//, m_fAgentRadius(5.0f)
{
}

CNavMeshAgent::CNavMeshAgent(const CNavMeshAgent& rhs)
	: Super(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_vMaxLinearSpeed(rhs.m_vMaxLinearSpeed)
	, m_fAgentRadius(rhs.m_fAgentRadius)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CNavMeshAgent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNavMeshAgent::Initialize(void* pArg)
{
	m_pTransform = GetTransform();
	m_pTransform->SetScale(m_fAgentRadius * 2.0f * Vec3::One);

	if (nullptr != pArg)
	{
		NAVIGATION_DESC* pDesc = reinterpret_cast<NAVIGATION_DESC*>(pArg);
		m_pCurrentCell = pDesc->pStartCell;
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

	// m_vLinearSpeed = m_vMaxLinearSpeed;

#pragma region AStarPerformance
	/*if (FAILED(m_pGameInstance->Add_Timer(TEXT("Timer_AStar"))))
		return E_FAIL;*/
#pragma endregion AStarPerformance

	return S_OK;
}

void CNavMeshAgent::Tick(_float fTimeDelta)
{
	ForceHeight();
	if (true == IsMoving() || false == IsOutOfWorld(m_vPrePos))
	{
		Slide(Move(fTimeDelta));
	} PopPath();
}

void CNavMeshAgent::LateTick(_float fTimeDelta)
{
	if (false == m_isMovingDirectly)
	{
		if (true == AdjustLocation() && true == AStar())
		{
			FunnelAlgorithm();
		}

		for (_int i = 0; i < m_dqPath.size(); ++i)
		{
			if (true == m_dqPath[i].first->isDead && true == AStar() && true == FunnelAlgorithm())
			{
				break;
			}
		}
	}

	m_vPrePos = m_pTransform->GetPosition();
}

void CNavMeshAgent::DebugRender(_bool bRenderPathCells, _bool bRenderEntries, _bool bRenderWayPoints)
{
	m_pBatch->Begin();
	if (true == bRenderPathCells && false == m_dqPath.empty())
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

	if (true == bRenderEntries && false == m_dqEntries.empty())
	{
		for (_int i = 0; i < m_dqEntries.size(); ++i)
		{
			const auto& [vLeft, vRight] = m_dqEntries[i];
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

	if (true == bRenderWayPoints && false == m_dqWayPoints.empty())
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

_bool CNavMeshAgent::IsIdle()
{
	return !m_isMoving;
}

_bool CNavMeshAgent::IsMoving()
{
	return m_isMoving;
}

_bool CNavMeshAgent::IsOutOfWorld(const Vec3& vPosition)
{
	return (vPosition.x >= 0.5f * gWorldCX || vPosition.x <= -0.5f * gWorldCX || vPosition.z >= 0.5f * gWorldCZ || vPosition.z <= -0.5f * gWorldCZ);
}

void CNavMeshAgent::ForceHeight()
{
	m_pTransform->Translate(Vec3(0.f, GetHeightOffset() + m_fAgentRadius, 0.f));
}

_float CNavMeshAgent::GetHeightOffset()
{
	Vec3 vPos = m_pTransform->GetPosition();

	if (nullptr == m_pCurrentCell)
		return 0.0f;

	const array<Vec3, POINT_END>* vPoints = &m_pCurrentCell->vPoints;

	Plane vPlane = Plane(
		m_pCurrentCell->vPoints[POINT_A],
		m_pCurrentCell->vPoints[POINT_B],
		m_pCurrentCell->vPoints[POINT_C]
	);

	return -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y + vPos.y);
}

void CNavMeshAgent::Slide(const Vec3 vPrePos)
{
	Cell* pNeighbor = nullptr;
	Vec3 vPosition = m_pTransform->GetPosition();

	if (nullptr == m_pCurrentCell || false == m_pCurrentCell->IsOut(vPosition, pNeighbor))
	{
		return;
	}

	_int iCount = 0;
	while (nullptr != pNeighbor && iCount < 10)
	{
		if (false == pNeighbor->IsOut(vPosition, pNeighbor))
		{
			m_pCurrentCell = pNeighbor;
			return;
		}
		++iCount;
	}

	Vec3 vDir = vPosition - vPrePos;
	Vec3 vPassedLine = m_pCurrentCell->GetPassedEdgeNormal(vPosition);

	vPosition = vPrePos + vDir - (0.002f + vDir.Dot(vPassedLine)) * vPassedLine;
	m_pCurrentCell = FindCellByPosition(vPosition);

	if (nullptr == m_pCurrentCell)
	{
		Obst* pObst = FindObstByPosition(vPosition);
		if (nullptr != pObst)
		{
			vPosition = pObst->GetClosestPoint(vPosition, m_fAgentRadius + 0.001f);
			m_pCurrentCell = FindCellByPosition(vPosition);
			m_pTransform->SetPosition(vPosition);

			if (false == m_isMovingDirectly && true == AStar())
			{
				FunnelAlgorithm();
			}
			return;
		}
	}

	m_pTransform->SetPosition(vPosition);
}

Vec3 CNavMeshAgent::Move(_float fTimeDelta)
{
	if (true == m_isMovingDirectly)
	{
		return m_vPrePos;
	}

	Vec3 vDistance = Vec3::Zero;
	Vec3 vMoveAmount = Vec3::Zero;
	Vec3 vDirection = Vec3::Zero;

	if (1 >= m_dqWayPoints.size()) // already in destcell or no pathfinding
	{
		vDistance = m_vDestPos - m_vPrePos;
		vDistance.y = 0.0f;
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
		{	// end
			vMoveAmount = vDistance;

			ClearWayPoints();

			m_isMoving = false;
		}
	}
	else
	{
		vDistance = m_dqWayPoints[1] - m_vPrePos;
		vDistance.y = 0.0f;
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (false == m_dqWayPoints.empty())
		{
			if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
			{	// to next waypoint
				m_dqWayPoints.pop_front();

				if (Vec3::Zero != vDirection)
				{
					_float fRight = m_pTransform->GetRight().Length();
					_float fUp = m_pTransform->GetUp().Length();
					_float fLook = m_pTransform->GetForward().Length();

					vDirection *= fLook;
					m_pTransform->SetForward(vDirection);

					Vec3 vRight = Vec3::UnitY.Cross(vDirection);
					vRight.Normalize();
					vRight *= fRight;
					m_pTransform->SetRight(vRight);

					Vec3 vUp = vDirection.Cross(vRight);
					vUp.Normalize();
					vUp *= fUp;
					m_pTransform->SetUp(vUp);
				}
			}
			else
			{
				m_dqWayPoints[0] = m_pTransform->GetPosition();
			}
		}
	}

	m_pTransform->Translate(vMoveAmount);

	return m_vPrePos;
}

_bool CNavMeshAgent::AStar()
{
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> Open;
	unordered_map<Cell*, PATH> Path;
	unordered_set<Cell*> Closed;

	ClearWayPoints();

	// start node
	Vec3 vStartPos = m_pTransform->GetPosition();

	Vec3 vDistance = m_vDestPos - vStartPos;
	_float h = vDistance.Length();

	Open.push(PQNode{ h, 0.0f, m_pCurrentCell });
	Path[m_pCurrentCell] = PATH(m_pCurrentCell, LINE_END);

	while (false == Open.empty())
	{
		PQNode tNode = Open.top();
		Cell* pCurrent = tNode.pCell;

		if (nullptr == pCurrent)
		{
			/*m_isMoving = true;
			m_pDestCell = m_dqPath.back().first;
			m_vDestPos = m_dqPath.back().first->GetCenter();*/
			return false;
		}

		if (pCurrent == m_pDestCell)
		{
			PATH& tNext = Path[m_pDestCell];
			auto& [pNext, ePassed] = tNext;

			m_dqEntries.push_front(pair(
				m_vDestPos,
				m_vDestPos
			));

			m_dqExpandedVertices.push_front(m_dqEntries.front());

			while (m_pCurrentCell != pNext)
			{
				m_dqPath.push_front(tNext);

				m_dqEntries.push_front(pair(
					pNext->vPoints[ePassed],
					pNext->vPoints[(ePassed + 1) % POINT_END]
				));

				m_dqExpandedVertices.push_front(m_dqEntries.front());

				/*if (m_pCurrentCell == pNext)
					break;*/

				tNext = Path[pNext];
			}

			const Vec3& vStartPos = m_pTransform->GetPosition();

			m_dqEntries.push_front(pair(
				vStartPos,
				vStartPos
			));

			m_dqExpandedVertices.push_front(m_dqEntries.front());

			m_isMoving = true;

			return true;
		}

		Open.pop();
		Closed.emplace(pCurrent);

		for (uint8 i = LINE_AB; i < LINE_END; ++i)
		{
			Cell* pNeighbor = pCurrent->pNeighbors[i];	// parent의 인접셀이 nullptr이 아니라면
			if (nullptr == pNeighbor)
			{
				continue;
			}

			// portal length
			// 4.1 Width Calculation
			auto& [pParent, ePassedLine] = Path[pCurrent];

			LINES eLine1 = LINE_END;

			_float fHalfWidth = -FLT_MAX;

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

				fHalfWidth = pCurrent->CalculateHalfWidth(eLine1, (LINES)(i));
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
				neighbor_g = tNode.g + Cell::CostBetweenPoint2Edge(
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

				//Vec3 vEdgeMid = 0.5f * (pCurrent->vPoints[i] + pCurrent->vPoints[(i + 1) % POINT_END]);
				Vec3 vClosestPoint2Edge = CNSHelper::ProjectionPoint2Edge(m_vDestPos, pCurrent->vPoints[i], pCurrent->vPoints[(i + 1) % POINT_END]);

				Open.push(PQNode{
					//neighbor_g + Cell::HeuristicCostEuclidean(vEdgeMid, m_vDestPos),
					neighbor_g + Cell::HeuristicCostEuclidean(vClosestPoint2Edge, m_vDestPos),
					neighbor_g,
					pNeighbor
					});
			}
		}
	}

	m_dqPath.clear();
	m_dqEntries.clear();
	m_dqExpandedVertices.clear();

	return false;
}

_bool CNavMeshAgent::FunnelAlgorithm()
{
	//deque<pair<Vec3, Vec3>> dqOffset;
	m_dqOffset.emplace_back(Vec3::Zero, Vec3::Zero); // for DebugRender

	for (_int i = 2; i < m_dqEntries.size(); ++i)
	{
		const auto& [vLeft0, vRight0] = m_dqEntries[i - 2];
		const auto& [vLeft1, vRight1] = m_dqEntries[i - 1];
		const auto& [vLeft2, vRight2] = m_dqEntries[i];

		Vec3 vLineL0 = vLeft1 - vLeft0;
		Vec3 vLineL1 = vLeft2 - vLeft1;

		Vec3 vLineR0 = vRight1 - vRight0;
		Vec3 vLineR1 = vRight2 - vRight1;

		if (Vec3::Zero == vLineL0)
		{
			if (Vec3::Zero != vLineL1)
			{
				vLineL1.Normalize();
			}
		}
		else
		{
			if (Vec3::Zero == vLineL1)
			{
				vLineL0.Normalize();
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
				vLineR1.Normalize();
			}
		}
		else
		{
			if (Vec3::Zero == vLineR1)
			{
				vLineR0.Normalize();
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
			vPerpendL = Vec3((m_fAgentRadius + EPSILON) * vAvgL.z, vAvgL.y, (m_fAgentRadius + EPSILON) * -vAvgL.x);
		}
		else
		{
			vPerpendL = m_dqOffset.back().first;
		}

		if (Vec3::Zero != vAvgR)
		{
			vAvgR.Normalize();
			vPerpendR = Vec3((m_fAgentRadius + EPSILON) * -vAvgR.z, vAvgR.y, (m_fAgentRadius + EPSILON) * vAvgR.x);
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
		const auto& [vLeft, vRight] = m_dqEntries[i];

		if (CNSHelper::TriArea2x(vPortalApex, vPortalRight, vRight) <= 0.0f)
		{
			if (vPortalApex == vPortalRight || CNSHelper::TriArea2x(vPortalApex, vPortalLeft, vRight) > 0.0f)
			{
				vPortalRight = vRight;				// funnel 당기기
				iRightIndex = i;
			}
			else
			{
				m_dqWayPoints.push_back(vPortalLeft + m_dqOffset[iLeftIndex].first);	// Right가 Left를 넘었다면 Left를 waypoint에 추가				

				vPortalApex = vPortalLeft;				// L을 새로운 시작점으로
				iApexIndex = iLeftIndex;

				vPortalRight = vPortalApex;				// 초기화
				iRightIndex = iApexIndex;

				i = iApexIndex;							// 재시작 인덱스
				continue;
			}
		}

		if (CNSHelper::TriArea2x(vPortalApex, vPortalLeft, vLeft) >= 0.0f)
		{
			if (vPortalApex == vPortalLeft || CNSHelper::TriArea2x(vPortalApex, vPortalRight, vLeft) < 0.0f)
			{
				vPortalLeft = vLeft;
				iLeftIndex = i;
			}
			else
			{
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

	return true;
}

_bool CNavMeshAgent::SetPath(const Vec3& vDestPos)
{
	//m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));

	m_pCurrentCell = FindCellByPosition(m_pTransform->GetPosition());	// TODO: ...
	m_pDestCell = FindCellByPosition(vDestPos);

	//m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));

	if (nullptr != m_pDestCell)
	{
		m_vDestPos = vDestPos;

		if (true == AStar())
		{
			/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
			fAStarPerformance = fAStarPerformance;*/
			return FunnelAlgorithm();
		}
		else
		{
			return false;
		}
	}

	Obst* pObst = FindObstByPosition(vDestPos);

	if (nullptr != pObst)
	{
		m_vDestPos = pObst->GetClosestPoint(vDestPos, m_fAgentRadius + 0.1f);
		m_pDestCell = FindCellByPosition(m_vDestPos);

		if (true == AStar())
		{
			/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
			fAStarPerformance = fAStarPerformance;*/
			return FunnelAlgorithm();
		}
		else
		{
			return false;
		}

		m_vDestPos = m_pTransform->GetPosition();
	}

	/*volatile _float fAStarPerformance = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_AStar"));
	fAStarPerformance = fAStarPerformance;*/
	return false;
}

void CNavMeshAgent::SetMoveDirectly(_bool isMovingDirectly)
{
	if (isMovingDirectly != m_isMovingDirectly)
	{
		m_isMovingDirectly = isMovingDirectly;
		if (true == isMovingDirectly)
		{
			ClearWayPoints();
		}
	}
}

void CNavMeshAgent::ClearWayPoints()
{
	m_dqWayPoints.clear();
	m_dqPath.clear();
	m_dqEntries.clear();
	m_dqOffset.clear();
	m_dqExpandedVertices.clear();
}

void CNavMeshAgent::Input(_float fTimeDelta)
{
}

_bool CNavMeshAgent::AdjustLocation()
{
	if (nullptr == m_pCurrentCell || true == m_pCurrentCell->isDead)
	{
		Vec3 vPosition = m_pTransform->GetPosition();
		m_pCurrentCell = FindCellByPosition(vPosition);
		if (nullptr == m_pCurrentCell)
		{
			Obst* pObst = FindObstByPosition(vPosition);
			if (nullptr != pObst)
			{
				vPosition = pObst->GetClosestPoint(vPosition, m_fAgentRadius + EPSILON);
				m_pCurrentCell = FindCellByPosition(vPosition);
				m_pTransform->SetPosition(vPosition);

				return true;
			}
			return false;
		}
		return true;
	}
	return false;
}

Cell* CNavMeshAgent::FindCellByPosition(const Vec3& vPosition)
{
	Cell* pCell = nullptr;

	_int iX = (vPosition.x + gWorldCX * 0.5f) / gGridCX;
	_int iZ = (vPosition.z + gWorldCZ * 0.5f) / gGridCZ;

	_int iKey = iZ * gGridX + iX;

	auto [begin, end] = m_pCellGrids->equal_range(iKey);

	for (auto cell = begin; cell != end;)
	{
		if (true == cell->second->isDead || nullptr == cell->second)
		{
			Safe_Delete(cell->second);
			cell = m_pCellGrids->erase(cell);
			continue;
		}
		else
		{
			if (false == cell->second->IsOut(vPosition, pCell))
			{
				return cell->second;
			}
		}

		++cell;
	}

	return nullptr;
}

Obst* CNavMeshAgent::FindObstByPosition(const Vec3& vPosition)
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

void CNavMeshAgent::PopPath()
{
	for (_int i = 0; i < m_dqPath.size(); ++i)
	{
		if (m_pCurrentCell == m_dqPath[i].first)
		{
			for (_int j = 0; j <= i; ++j)
			{
				if (false == m_dqPath.empty())				m_dqPath.pop_front();
				if (false == m_dqEntries.empty())			m_dqEntries.pop_front();
				if (false == m_dqOffset.empty())			m_dqOffset.pop_front();
				if (false == m_dqExpandedVertices.empty())	m_dqExpandedVertices.pop_front();
			}
			break;
		}
	}
}

CNavMeshAgent* CNavMeshAgent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavMeshAgent* pInstance = new CNavMeshAgent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavMeshAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavMeshAgent::Clone(CGameObject* pGameObject, void* pArg)
{
	CNavMeshAgent* pInstance = new CNavMeshAgent(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavMeshAgent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavMeshAgent::Free()
{
	Super::Free();
}