#include "Transform.h"
#include "GameObject.h"
#include "GameInstance.h"

CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, ComponentType type)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned(false)
	, m_eType(type)
	, m_pGameObject(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = GET_INSTANCE(CGameInstance);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
	, m_eType(rhs.m_eType)
	, m_pGameObject(rhs.m_pGameObject)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	return S_OK;
}

CGameObject* CComponent::GetGameObject() const
{
	return m_pGameObject;
}

CTransform* CComponent::GetTransform() const
{
	return m_pGameObject->GetTransform();
}


void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	RELEASE_INSTANCE(CGameInstance);
}
