#include "MonoBehaviour.h"

CMonoBehaviour::CMonoBehaviour(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext, ComponentType::Script)
{
	this->m_pGameInstance = Super::m_pGameInstance;
}

CMonoBehaviour::CMonoBehaviour(const CMonoBehaviour& rhs)
	:Super(rhs)
	, m_pGameInstance(rhs.m_pGameInstance)
{

}

void CMonoBehaviour::Tick(_float fTimeDelta)
{
}

void CMonoBehaviour::LateTick(_float fTimeDelta)
{
}

void CMonoBehaviour::DebugRender()
{
}

void CMonoBehaviour::Free()
{
	Super::Free();
}
