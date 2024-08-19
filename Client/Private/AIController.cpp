#include "stdafx.h"
#include "AIController.h"
#include "GameInstance.h"
#include "GameObject.h"

CAIController::CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(70.0f, 70.0f, 70.0f))
	, m_fAgentRadius(3.4f)
{
}

CAIController::CAIController(const CAIController& rhs)
	: Super(rhs)
	, m_vLinearSpeed(rhs.m_vLinearSpeed)
	, m_fAgentRadius(rhs.m_fAgentRadius)
{
}

HRESULT CAIController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAIController::Initialize(void* pArg)
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

	return S_OK;
}

void CAIController::Tick(_float fTimeDelta)
{
	AutoMove(fTimeDelta);
}

void CAIController::LateTick(_float fTimeDelta)
{
}

void CAIController::DebugRender()
{
}

void CAIController::SetRadius(const _float fRadius)
{
	m_fAgentRadius = fRadius;
	m_pGameObject->GetNavMeshAgent()->SetRadius(m_fAgentRadius);
}

void CAIController::SetLinearSpeed(const Vec3& vLinearSpeed)
{
	m_vLinearSpeed = vLinearSpeed;
	m_pGameObject->GetNavMeshAgent()->SetLinearSpeed(m_vLinearSpeed);
}

void CAIController::AutoMove(_float fTimeDelta)
{
	if (m_vecWayPoints.size() < 2)
	{
		return;
	}

	if (true == m_pGameObject->GetNavMeshAgent()->IsIdle())
	{
		if (++m_iCurrentWayIdx >= m_vecWayPoints.size())
		{
			m_iCurrentWayIdx %= m_vecWayPoints.size();
		}

		if (false == m_pGameObject->GetNavMeshAgent()->SetPath(m_vecWayPoints[m_iCurrentWayIdx]))
		{
			++m_iCurrentWayIdx %= m_vecWayPoints.size();
		}
	}
}

CAIController* CAIController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAIController* pInstance = new CAIController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAIController::Clone(CGameObject* pGameObject, void* pArg)
{
	CAIController* pInstance = new CAIController(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAIController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAIController::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	m_vecWayPoints.clear();

	Super::Free();
}
