#include "stdafx.h"
#include "AgentController.h"
#include "GameInstance.h"
#include "GameObject.h"

constexpr auto EPSILON = 0.001f;

CAgentController::CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
	, m_vLinearSpeed(Vec3(6.3f, 6.3f, 6.3f))
	, m_vMaxLinearSpeed(Vec3(20.f, 20.f, 20.f))
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
	m_pNavMeshAgent = m_pGameObject->GetNavMeshAgent();

	return S_OK;
}

void CAgentController::Tick(_float fTimeDelta)
{
	if (m_vNetMove.Length() > EPSILON)
		Move(fTimeDelta);

	if (!m_pNavMeshAgent->Walkable(m_pTransform->GetPosition()))
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
	if (!IsMoving())
	{
		return true;
	}
}

_bool CAgentController::IsMoving()
{
	if (KEY_PRESSING(KEY::W) || KEY_DOWN(KEY::W) || KEY_PRESSING(KEY::A) || KEY_DOWN(KEY::A) ||
		KEY_PRESSING(KEY::S) || KEY_DOWN(KEY::S) || KEY_PRESSING(KEY::D) || KEY_DOWN(KEY::D))
	{
		return true;
	}

	return false;
}

void CAgentController::Move(_float fTimeDelta)
{
	m_vNetMove.Normalize();
	Vec3 vSpeed = fTimeDelta * m_vLinearSpeed * m_vNetMove;
	m_pTransform->Translate(vSpeed);

	m_vNetMove = Vec3::Zero;
}

_bool CAgentController::Pick(_uint screenX, _uint screenY, OUT Vec3& pickPos, OUT _float& distance)
{
	Matrix matBoard(1.f, 0, 0, 0,
					0, 1.f, 0, 0,
					0, 0, 1.f, 0,
					0, m_pTransform->GetPosition().y, 0, 1.f);
	return static_cast<CTerrain*>(m_pGameObject->GetFixedComponent(ComponentType::Terrain))->Pick(screenX, screenY, pickPos, distance, matBoard);
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
