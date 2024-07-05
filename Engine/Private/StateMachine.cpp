#include "StateMachine.h"
#include "GameObject.h"
#include "State.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{

}

CStateMachine::CStateMachine(const CStateMachine& rhs)
	:Super(rhs)
	, m_hashStates(rhs.m_hashStates)
	, m_pCurrState(rhs.m_pCurrState)
{
}

HRESULT CStateMachine::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{

	return S_OK;
}

void CStateMachine::Tick(_float fTimeDelta)
{
	if (!m_pCurrState)
		__debugbreak();
		//return;

	m_pCurrState->Tick(fTimeDelta);
}

void CStateMachine::LateTick(_float fTimeDelta)
{
	if (!m_pCurrState)
		__debugbreak();

	const wstring& strState = m_pCurrState->LateTick(fTimeDelta);

	if(m_pCurrState->GetName() != strState)
		ChangeState(strState);
}

void CStateMachine::DebugRender()
{
}

HRESULT CStateMachine::AddState(CState* pState)
{
	STATES::iterator iter = m_hashStates.find(pState->GetName());

	if (iter != m_hashStates.end())
		return E_FAIL;

	m_hashStates.emplace(pState->GetName(), pState);

	if (nullptr == m_pCurrState)
	{
		if (FAILED(ChangeState(pState->GetName())))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine::ChangeState(const wstring& strState, _int iAnimIndex)
{
	STATES::iterator iter = m_hashStates.find(strState);

	if (iter == m_hashStates.end())
		return E_FAIL;

	if (m_pCurrState)
	{
		iter->second->Exit();
		Safe_Release(m_pCurrState);
	}

	m_pCurrState = iter->second;
	Safe_AddRef(m_pCurrState);

	m_pCurrState->Enter(iAnimIndex);

	return S_OK;
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(CGameObject* pGameObject, void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	Super::Free();
}
