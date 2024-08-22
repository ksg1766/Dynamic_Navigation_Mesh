#include "stdafx.h"
#include "AgentController.h"
#include "GameInstance.h"
#include "Agent.h"
#include "Obstacle.h"
#include "MainCamera.h"
#include "MainCameraController.h"
#include "NSHelper.h"
#include "Terrain.h"
#include "StaticBase.h"
#include "DebugDraw.h"

CAgentController::CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(80.0f, 80.0f, 80.0f))
	, m_fAgentRadius(2.7f)
{
}

CAgentController::CAgentController(const CAgentController& rhs)
	:Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
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

	m_pGameObject->GetNavMeshAgent()->SetRadius(m_fAgentRadius);
	m_pGameObject->GetNavMeshAgent()->SetLinearSpeed(m_vLinearSpeed);

	CStaticBase* pHoldingObst = nullptr;

	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Bus"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Bus"), pHoldingObst);

	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Dumpster"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Dumpster"), pHoldingObst);

	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Light_Lamppost"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Light_Lamppost"), pHoldingObst);
	
	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Picnic_Table"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Picnic_Table"), pHoldingObst);

	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Receptacle_Recycling"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Receptacle_Recycling"), pHoldingObst);
	
	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Signal_Separated"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Signal_Separated"), pHoldingObst);
	
	pHoldingObst = CStaticBase::Create(m_pDevice, m_pContext);
	pHoldingObst->SetObjectTag(TEXT("Signal_Separated_No_Stoplight"));
	pHoldingObst->Initialize(nullptr);
	m_HoldingObstacles.emplace_back(TEXT("Signal_Separated_No_Stoplight"), pHoldingObst);

#pragma region AStarPerformance
	/*if (FAILED(m_pGameInstance->Add_Timer(TEXT("Timer_AStar"))))
		return E_FAIL;*/
#pragma endregion AStarPerformance

	return S_OK;
}

void CAgentController::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);
}

void CAgentController::LateTick(_float fTimeDelta)
{
	
}

_bool CAgentController::IsIdle()
{
	return !m_isMoving;
}

_bool CAgentController::IsMoving()
{
	return m_isMoving;
}

_bool CAgentController::Pick(CTerrain* pTerrain, _uint screenX, _uint screenY)
{
	if (VIEWMODE::THIRD != m_eViewMode)
	{
		_float fDistance = 0.0f;
		Vec3 vPickedPos = Vec3::Zero;

		if (true == pTerrain->Pick(screenX, screenY, vPickedPos, fDistance, pTerrain->GetTransform()->WorldMatrix()))
		{
			return m_pGameObject->GetNavMeshAgent()->SetPath(vPickedPos);
		}
	}

	return false;
}

void CAgentController::SetRadius(const _float fRadius)
{
	m_fAgentRadius = fRadius;
	m_pGameObject->GetNavMeshAgent()->SetRadius(m_fAgentRadius);
	m_pTransform->SetScale(m_fAgentRadius * 2.0f * Vec3::One);
}

void CAgentController::SetLinearSpeed(const Vec3& vLinearSpeed)
{
	m_vLinearSpeed = vLinearSpeed;
	m_pGameObject->GetNavMeshAgent()->SetLinearSpeed(m_vLinearSpeed);
}

void CAgentController::Input(_float fTimeDelta)
{
	if (KEY_DOWN(KEY::V))
	{
		m_eViewMode = VIEWMODE(((uint8)m_eViewMode + 1U) % (uint8)VIEWMODE::MODE_END);
		m_pGameInstance->ChangeCamera();

		if (VIEWMODE::THIRD == m_eViewMode)
		{
			dynamic_cast<CMainCamera*>(m_pGameInstance->GetCurrentCamera())->GetController()->SetTarget(m_pTransform);
			m_pGameObject->GetNavMeshAgent()->SetMoveDirectly(true);
			::ShowCursor(false);
		}
		else
		{
			m_pGameObject->GetNavMeshAgent()->SetMoveDirectly(false);
			::ShowCursor(true);
		}
	}

	if (VIEWMODE::THIRD == m_eViewMode)
	{
		_long		dwMouseMove = 0;

		if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransform->RotateYAxisFixed(Vec3(0.0f, 12 * dwMouseMove * fTimeDelta, 0.0f));
		}

		if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransform->RotateYAxisFixed(Vec3(12 * dwMouseMove * fTimeDelta, 0.0f, 0.0f));
		}

		MoveDirectly(fTimeDelta);
		PlaceObstacle();
		
		::SetCursorPos(g_iWinSizeX / 2, g_iWinSizeY / 2);
	}
}

_bool CAgentController::MoveDirectly(_float fTimeDelta)
{
	if (KEY_PRESSING(KEY::UP_ARROW) || KEY_DOWN(KEY::UP_ARROW))
		m_vNetMove += m_pTransform->GetForward();

	if (KEY_PRESSING(KEY::DOWN_ARROW) || KEY_DOWN(KEY::DOWN_ARROW))
		m_vNetMove -= m_pTransform->GetForward();
	
	if (KEY_PRESSING(KEY::LEFT_ARROW) || KEY_DOWN(KEY::LEFT_ARROW))
		m_vNetMove -= m_pTransform->GetRight();	

	if (KEY_PRESSING(KEY::RIGHT_ARROW) || KEY_DOWN(KEY::RIGHT_ARROW))
		m_vNetMove += m_pTransform->GetRight();

	if (Vec3::Zero != m_vNetMove)
	{
		m_pGameObject->GetNavMeshAgent()->SetState(true);

		m_vNetMove.Normalize();
		m_vNetMove *= (fTimeDelta * m_vLinearSpeed);

		m_pTransform->Translate(m_vNetMove);
		m_vNetMove = Vec3::Zero;

		return true;
	}

	return false;
}

void CAgentController::PlaceObstacle()
{
	if (KEY_DOWN(KEY::F1))
		(0 != m_iObstacleIndex) ? m_iObstacleIndex = 0 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F2))
		(1 != m_iObstacleIndex) ? m_iObstacleIndex = 1 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F3))
		(2 != m_iObstacleIndex) ? m_iObstacleIndex = 2 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F4))
		(3 != m_iObstacleIndex) ? m_iObstacleIndex = 3 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F5))
		(4 != m_iObstacleIndex) ? m_iObstacleIndex = 4 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F6))
		(5 != m_iObstacleIndex) ? m_iObstacleIndex = 5 : m_iObstacleIndex = -1;
	else if (KEY_DOWN(KEY::F7))
		(6 != m_iObstacleIndex) ? m_iObstacleIndex = 6 : m_iObstacleIndex = -1;

	if (0 <= m_iObstacleIndex)
	{
		auto& [Name, Object] = m_HoldingObstacles[m_iObstacleIndex];
		Vec3 vPlacePosition = m_pTransform->GetPosition();

		Vec3 vLook = -m_pTransform->GetForward();
		vLook.y = 0.0f;
		vLook.Normalize();

		Vec3 vRight = Vec3::Up.Cross(vLook);
		vRight.Normalize();

		Vec3 vUp = vLook.Cross(vRight);
		vUp.Normalize();;

		Object->GetTransform()->SetRight(vRight);
		Object->GetTransform()->SetUp(vUp);
		Object->GetTransform()->SetForward(vLook);

		vPlacePosition += 18.0f * -vLook;
		vPlacePosition.y = 0.0f;

		Object->GetTransform()->SetPosition(vPlacePosition);

		Object->GetRenderer()->Add_RenderGroup(CRenderer::RG_NONBLEND_INSTANCE, Object);

		if (m_pGameInstance->Mouse_Up(DIM_LB))
		{
			Matrix& matObst = Object->GetTransform()->WorldMatrix();

			DLG_PlaceObstacle(Name, vPlacePosition, matObst);
		}
		else if (m_pGameInstance->Mouse_Up(DIM_RB))
		{
			m_iObstacleIndex = -1;
		}
	}
	else
	{
		if (m_pGameInstance->Mouse_Up(DIM_RB))
		{
			Vec3 vLook = m_pTransform->GetForward();
			vLook.y = 0.0f;
			vLook.Normalize();

			Vec3 vTargetPosition = m_pTransform->GetPosition() + 18.0f * vLook;
			vTargetPosition.y = 0.0f;

			Obst* pObst = m_pGameObject->GetNavMeshAgent()->FindObstByPosition(vTargetPosition);
			
			if (nullptr != pObst)
			{
				for (_int i = 0; i < m_HoldingObstacles.size(); ++i)
				{
					if (pObst->pGameObject->GetObjectTag() == m_HoldingObstacles[i].first)
					{
						m_iObstacleIndex = i;
						break;
					}
				}

				DLG_RemoveObstacle(*pObst);
			}
		}
		else if(m_pGameInstance->Mouse_Pressing(DIM_RB))
		{
			m_bTargetCursor = true;
		}
	}
}

void CAgentController::DebugRender()
{
	if (true == m_bTargetCursor)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());

		m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

		m_pEffect->Apply(m_pContext);
		m_pContext->IASetInputLayout(m_pInputLayout);

		m_pBatch->Begin();

		Vec3 vLook = m_pTransform->GetForward();
		vLook.y = 0.0f;
		vLook.Normalize();
		vLook *= 18.0f;
		vLook.y = 0.25f;

		Vec3 vTargetPos = m_pTransform->GetPosition() + vLook;

		DX::DrawRing(m_pBatch, vTargetPos, 1.0f * Vec3::UnitX, 1.0f * Vec3::UnitZ, Colors::Red);
		DX::DrawRing(m_pBatch, vTargetPos, 2.2f * Vec3::UnitX, 2.2f * Vec3::UnitZ, Colors::IndianRed);

		m_pBatch->End();

		m_bTargetCursor = false;
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

	for (auto& pair : m_HoldingObstacles)
	{
		Safe_Release(pair.second);
	} m_HoldingObstacles.clear();

	Super::Free();
}