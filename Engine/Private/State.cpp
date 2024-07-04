#include "State.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "MonoBehaviour.h"

CState::CState()
{
	m_pGameInstance = GET_INSTANCE(CGameInstance);
}

CState::CState(const CState& rhs)
	:Super(rhs)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController)
{
	m_pGameObject = pGameObject;
	m_pModel = m_pGameObject->GetModel();
	m_iCurrAnimation = 0;
	m_pController = pController;

	for (const wstring& strAnim : tStateAnim.vecAnimations)
	{
		_int iAnimIndex = m_pModel->GetAnimationIndexByName(strAnim);
		_float fAnimTimes = m_pModel->GetAnimationTimeByIndex(iAnimIndex);
		if (iAnimIndex < 0)
			return E_FAIL;

		m_vecAnimIndexTime.push_back(make_pair(iAnimIndex, fAnimTimes));
	}

	return S_OK;
}

HRESULT CState::Enter(_int i)
{
	m_fTimeSum = 0.f;
	m_pModel->SetNextAnimationIndex(i);	// TODO: 루프 설정 아직 안함

	return S_OK;
}

void CState::Free()
{
	Super::Free();
	GET_INSTANCE(CGameInstance);
}