#include "stdafx.h"
#include "MonsterStats.h"
#include "GameInstance.h"
#include "GameObject.h"

CMonsterStats::CMonsterStats(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

CMonsterStats::CMonsterStats(const CMonsterStats& rhs)
	:Super(rhs)
{
}

HRESULT CMonsterStats::Initialize(void* pArg)
{
	MONSTERSTAT* pStat = reinterpret_cast<MONSTERSTAT*>(pArg);
	m_iMaxHP	= pStat->iMaxHP;
	m_iHP		= m_iMaxHP;
	m_iDPS		= pStat->iDPS;

	return S_OK;
}

CMonsterStats* CMonsterStats::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterStats* pInstance = new CMonsterStats(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterStats");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMonsterStats::Clone(CGameObject* pGameObject, void* pArg)
{
	CMonsterStats* pInstance = new CMonsterStats(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMonsterStats");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterStats::Free()
{
	Super::Free();
}