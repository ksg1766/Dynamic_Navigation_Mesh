#include "..\Public\Transform.h"
#include "GameObject.h"

CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, ComponentType type)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_eType(type)
	, m_pGameObject(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_eType(rhs.m_eType)
	, m_pGameObject(rhs.m_pGameObject)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

CGameObject* CComponent::GetGameObject()
{
	return m_pGameObject;
}

CTransform* CComponent::GetTransform()
{
	return m_pGameObject->GetTransform();
}
