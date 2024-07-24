#include "stdafx.h"
#include "AgentController.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Terrain.h"
#include "CellData.h"

constexpr auto EPSILON = 0.001f;

CAgentController::CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(50.0f, 50.0f, 50.0f))
	, m_vMaxLinearSpeed(Vec3(100.0f, 100.0f, 100.0f))
{
}

CAgentController::CAgentController(const CAgentController& rhs)
	:Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_vMaxLinearSpeed(rhs.m_vMaxLinearSpeed)
{
}

HRESULT CAgentController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAgentController::Initialize(void* pArg)
{
	m_pTransform = GetTransform();

	if (nullptr != pArg)
	{
		m_pCurrentCell = static_cast<CellData*>(pArg);
	}

	return S_OK;
}

void CAgentController::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (true == CanMove(m_pTransform->GetPosition()))
	{
		for (_int i = m_vecPath.size() - 1; i >= 0; --i)
		{
			if (m_pCurrentCell == m_vecPath[i])
			{
				for (_int j = m_vecPath.size() - 1; j > i; --j)
				{
					m_vecPath.pop_back();
				}
			}
		}
	}
	else
	{
		m_pTransform->SetPosition(m_vPrePos);
		// sliding = move - (move · 충돌 edge normal) * 충돌 edge normal
		/*Vec3 vDir = m_pTransform->GetPosition() - m_vPrePos;
		Vec3 vPassedLine = m_pNavMeshAgent->GetPassedEdgeNormal(m_pTransform->GetPosition());

		m_pTransform->SetPosition(m_vPrePos + vDir - vDir.Dot(vPassedLine) * vPassedLine);*/
	}

}

void CAgentController::LateTick(_float fTimeDelta)
{
	m_vPrePos = m_pTransform->GetPosition();
}

void CAgentController::DebugRender()
{
}

_bool CAgentController::IsIdle()
{
	return !IsMoving();
}

_bool CAgentController::IsMoving()
{
	return m_isMoving;
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

_bool CAgentController::CanMove(Vec3 vPoint)
{
	CellData* pNeighbor = nullptr;

	if (true == m_pCurrentCell->IsOut(vPoint, pNeighbor))
	{
		if (nullptr != pNeighbor)
		{
			while (true)
			{
				if (nullptr == pNeighbor)
					return false;

				if (false == pNeighbor->IsOut(vPoint, pNeighbor))
				{
					m_pCurrentCell = pNeighbor;
					break;
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

_bool CAgentController::AStar()	// 매번 호출되는게 아님 혼동하지 말자...
{
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pqOpen;
	map<CellData*, CellData*> mapPath;
	map<CellData*, _float> mapCost;
	set<CellData*> setClosed;
	m_vecPath.clear();

	{	// start node
		Vec3 vStartPos = m_pTransform->GetPosition();

		_float g = 0.0f;
		Vec3 vDistance = m_vDestPos - vStartPos;
		_float h = vDistance.Length();

		pqOpen.push(PQNode{ g + h, g, m_pCurrentCell });
		mapCost[m_pCurrentCell] = 0.0f;
		mapPath[m_pCurrentCell] = nullptr;
	}

	while (false == pqOpen.empty())
	{
		PQNode tNode = pqOpen.top();

		if (tNode.pCell == m_pDestCell)
		{
			CellData* pCell = m_pDestCell;
			while (nullptr != pCell)
			{
				m_vecPath.push_back(pCell);
				pCell = mapPath[pCell];
			}

			return true;
		}

		pqOpen.pop();
		setClosed.emplace(tNode.pCell);

		for (_int i = LINE_AB; i < LINE_END; ++i)
		{
			CellData* pNeighbor = tNode.pCell->pNeighbors[i];
			if (nullptr != pNeighbor && 0 == setClosed.count(pNeighbor))
			{
				mapCost[pNeighbor] = mapCost[tNode.pCell] + CellData::CostBetween(tNode.pCell, pNeighbor);
				mapPath[pNeighbor] = tNode.pCell;

				_float g = mapCost[pNeighbor];
				pqOpen.push(PQNode(g + CellData::HeuristicCost(pNeighbor, m_vDestPos), g, pNeighbor));
			}
		}
	}

	// 가장 가까운 노드 반환하도록
	return false;
}

void CAgentController::Move(_float fTimeDelta)
{
	if (false == IsMoving()) { return; }

	Vec3 vDistance = Vec3::Zero;
	Vec3 vMoveAmount = Vec3::Zero;
	Vec3 vDirection = Vec3::Zero;

	// waypoint는 임시로 무게중심.
	if (1 >= m_vecPath.size()) // already in destcell or no pathfinding
	{
		vDistance = m_vDestPos - m_pTransform->GetPosition();
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
		{	// end
			vMoveAmount = vDistance;
			m_vecPath.pop_back();
			m_isMoving = false;
		}
	}
	else
	{
		vDistance = m_vecPath[m_vecPath.size() - 2]->GetCenter() - m_pTransform->GetPosition();
		vDistance.Normalize(OUT vDirection);

		vMoveAmount = fTimeDelta * m_vLinearSpeed * vDirection;
		if (vMoveAmount.LengthSquared() > vDistance.LengthSquared())
		{	// to next waypoint
			m_vecPath.pop_back();
		}
	}
	
	m_pTransform->Translate(vMoveAmount);
}

CellData* CAgentController::FindCellByPosition(const Vec3& vPosition)
{	// 일단 brute force로 구현 후 개선.
	CellData* pCell = nullptr;

	for (_int i = 0; i < m_pCells->size(); ++i)
	{
		if (false == (*m_pCells)[i]->IsOut(vPosition, pCell))
		{
			return (*m_pCells)[i];
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

	m_pDestCell = FindCellByPosition(vPickedPos);

	if (nullptr == m_pDestCell)
	{
		return false;
	}

	m_vDestPos = vPickedPos;

	if (true == AStar())
	{
		m_isMoving = true;
		return true;
	}
	
	m_vDestPos = m_pTransform->GetPosition();
	m_pDestCell = nullptr;

	return false;
}

void CAgentController::Input(_float fTimeDelta)
{
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
	Super::Free();
}
