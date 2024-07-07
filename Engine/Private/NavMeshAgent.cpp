#include "NavMeshAgent.h"
#include "ComponentManager.h"
#include "GameInstance.h"
#include "LevelManager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "FileUtils.h"
#include "Cell.h"
#include "GameObject.h"
#include "LevelManager.h"
#include "DebugDraw.h"

#ifdef _DEBUG
_bool CNavMeshAgent::m_IsRendered = false;
#endif
vector<CCell*> CNavMeshAgent::m_Cells;

CNavMeshAgent::CNavMeshAgent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, ComponentType::NavMeshAgent)
{
}

CNavMeshAgent::CNavMeshAgent(const CNavMeshAgent& rhs)
	: Super(rhs)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CNavMeshAgent::Initialize_Prototype(const wstring& strNavigationData)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(strNavigationData, Read);

	_int iIndex = 0;
	while (true)
	{
		_float3 vPoints[3];
		::ZeroMemory(vPoints, 3 * sizeof(_float3));

		if (false == file->Read(vPoints))
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, iIndex);
		m_Cells.push_back(pCell);
		++iIndex;
	}

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t		iLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
		return E_FAIL;
	}
#endif

	return S_OK;
}

HRESULT CNavMeshAgent::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pNaviDesc = (NAVIGATION_DESC*)pArg;

	/*  이 네비게이션을 이용하고자하는 객체가 어떤 셀에 있는지 저장한다. */
	m_iCurrentIndex = pNaviDesc->iCurrentIndex;
	
	Vec3 vCenter = (*m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_A)
		+ *m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_B)
		+ *m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_C)) / 3.f;

	m_pTransform = GetTransform();
	m_pTransform->SetPosition(vCenter);
	ForceHeight();

	return S_OK;
}

void CNavMeshAgent::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//if (m_IsRendered)
	//	m_IsRendered = false;
#endif
}

void CNavMeshAgent::DebugRender()
{
#ifdef _DEBUG
	/*CLevelManager* pInstance = GET_INSTANCE(CLevelManager);
	if (2 == pInstance->GetCurrentLevelIndex())
	{
		if (m_IsRendered)
			return;

		m_pEffect->SetWorld(XMMatrixIdentity());

		CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

		m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

		RELEASE_INSTANCE(CPipeLine);

		m_pEffect->Apply(m_pContext);
		m_pContext->IASetInputLayout(m_pInputLayout);

		m_Cells[m_iCurrentIndex]->DebugRender(m_pBatch, Colors::YellowGreen);
		for (_int i = 0; i < m_Cells.size(); ++i)
		{
			if (m_Cells[i])
			{
				m_Cells[i]->DebugRender(m_pBatch, Colors::Cyan);
			}
		}

		m_IsRendered = true;
	}
	RELEASE_INSTANCE(CLevelManager)*/
#endif
}

_bool CNavMeshAgent::Walkable(_fvector vPoint)
{
	_int		iNeighborIndex = 0;

	if (true == m_Cells[m_iCurrentIndex]->isOut(vPoint, &iNeighborIndex))
	{
		/* 나간 방향에 이웃셀이 있으면 움직여야해! */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_Cells[iNeighborIndex]->isOut(vPoint, &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
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

void CNavMeshAgent::ForceHeight()
{
	m_pTransform->Translate(Vec3(0.f, GetHeightOffset(), 0.f));
}

_float CNavMeshAgent::GetHeightOffset()
{
	_float3 vPos(m_pTransform->GetPosition());

	const _float3* vPoints = m_Cells[m_iCurrentIndex]->Get_Points();

	_float4 vPlane;
	XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&vPoints[CCell::POINT_A]), XMLoadFloat3(&vPoints[CCell::POINT_B]), XMLoadFloat3(&vPoints[CCell::POINT_C])));

	return -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y + vPos.y);
}

_float3 CNavMeshAgent::GetPassedEdgeNormal(_fvector vPos)
{
	_int		iNeighborIndex = 0;

	if (true == m_Cells[m_iCurrentIndex]->isOut(vPos, &iNeighborIndex))
	{
		/* 나간 방향에 이웃셀이 있으면 움직여야해! */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return m_Cells[m_iCurrentIndex]->GetPassedEdgeNormal(vPos);

				if (false == m_Cells[iNeighborIndex]->isOut(vPos, &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					break;
				}
			}
			return _float3(0.f, 0.f, 0.f);
		}
		else
			return m_Cells[m_iCurrentIndex]->GetPassedEdgeNormal(vPos);
	}
	else
		return  _float3(0.f, 0.f, 0.f);
}

void CNavMeshAgent::SetCurrentIndex(_int iCurrentIndex)
{
	const _float3* vPoints = m_Cells[iCurrentIndex]->Get_Points();
	_float3 vCenter = (vPoints[0] + vPoints[1] + vPoints[2]) / 3.f;

	m_pGameObject->GetTransform()->SetPosition(vCenter);
	m_iCurrentIndex = iCurrentIndex;
}

HRESULT CNavMeshAgent::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavMeshAgent* CNavMeshAgent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationData)
{
	CNavMeshAgent* pInstance = new CNavMeshAgent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationData)))
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