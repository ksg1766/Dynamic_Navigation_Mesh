#include "stdafx.h"
#include "AgentController.h"
#include "GameInstance.h"
#include "Agent.h"
#include "Obstacle.h"
#include "NSHelper.h"
#include "Terrain.h"
#include "DebugDraw.h"

constexpr auto EPSILON = 0.0001f;

CAgentController::CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(100.0f, 100.0f, 100.0f))
	, m_fAgentRadius(3.4f)
	//, m_fAgentRadius(5.0f)
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

#pragma region AStarPerformance
	/*if (FAILED(m_pGameInstance->Add_Timer(TEXT("Timer_AStar"))))
		return E_FAIL;*/
#pragma endregion AStarPerformance

	return S_OK;
}

void CAgentController::Tick(_float fTimeDelta)
{
	
}

void CAgentController::LateTick(_float fTimeDelta)
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

_bool CAgentController::Pick(CTerrain* pTerrain, _uint screenX, _uint screenY)
{
	_float fDistance = 0.0f;
	Vec3 vPickedPos;

	if (true == pTerrain->Pick(screenX, screenY, vPickedPos, fDistance, pTerrain->GetTransform()->WorldMatrix()))
	{
		return m_pGameObject->GetNavMeshAgent()->SetPath(vPickedPos);
	}

	return false;
}

void CAgentController::SetRadius(const _float fRadius)
{
	m_fAgentRadius = fRadius;
}

void CAgentController::SetLinearSpeed(const Vec3& vLinearSpeed)
{
	m_vLinearSpeed = vLinearSpeed;
	m_pGameObject->GetNavMeshAgent();
}

void CAgentController::Input(_float fTimeDelta)
{
}

void CAgentController::DebugRender()
{
	/*m_pBatch->Begin();

	m_pBatch->End();*/
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

	Super::Free();
}
