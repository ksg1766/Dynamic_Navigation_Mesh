#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_IsDead(false)
	, m_IsInstance(false)
	, m_eLayerTag(LAYERTAG::LAYER_END)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = GET_INSTANCE(CGameInstance);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_IsDead(rhs.m_IsDead)
	, m_IsInstance(rhs.m_IsInstance)
	, m_eLayerTag(rhs.m_eLayerTag)
	, m_strObjectTag(rhs.m_strObjectTag)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_arrComponents)
		if(iter)	iter->Tick(fTimeDelta);
	for (auto& iter : m_vecScripts)
		iter->Tick(fTimeDelta);
}

void CGameObject::LateTick(const _float& fTimeDelta)
{
	for (auto& iter : m_arrComponents)
		if(iter)	iter->LateTick(fTimeDelta);
	for (auto& iter : m_vecScripts)
		iter->LateTick(fTimeDelta);
}

void CGameObject::DebugRender()
{
	for (auto& iter : m_arrComponents)
		if (iter)	iter->DebugRender();
	for (auto& iter : m_vecScripts)
		iter->DebugRender();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::RenderInstance()
{
	return S_OK;
}

CTransform* CGameObject::GetTransform()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Transform);
	return static_cast<CTransform*>(pComponent);
}

CRigidBody* CGameObject::GetRigidBody()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::RigidBody);
	return static_cast<CRigidBody*>(pComponent);
}

CVIBuffer* const CGameObject::GetBuffer()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Buffer);
	return static_cast<CVIBuffer*>(pComponent);
}

CShader* CGameObject::GetShader()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Shader);
	return static_cast<CShader*>(pComponent);
}

CModel* CGameObject::GetModel()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Model);
	return static_cast<CModel*>(pComponent);
}

CRenderer* CGameObject::GetRenderer()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Renderer);
	return static_cast<CRenderer*>(pComponent);
}

CTexture* CGameObject::GetTexture()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::Texture);
	return static_cast<CTexture*>(pComponent);
}

CNavMeshAgent* CGameObject::GetNavMeshAgent()
{
	CComponent* pComponent = GetFixedComponent(ComponentType::NavMeshAgent);
	return static_cast<CNavMeshAgent*>(pComponent);
}

CTransform* CGameObject::GetOrAddTransform(_uint iLevelIndex)
{
	if (GetTransform() == nullptr)
	{
		AddComponent(iLevelIndex, ComponentType::Transform, TEXT("Prototype_Component_Transform"));
	}

	return GetTransform();
}

CComponent* CGameObject::GetFixedComponent(const ComponentType& type)
{
	_uint index = static_cast<_uint>(type);
	assert(index <= FIXED_COMPONENT_COUNT);
	return m_arrComponents[index];
}

HRESULT CGameObject::AddComponent(_uint iLevelIndex, const ComponentType& type, const wstring& strPrototypeTag, void* pArg)
{
	_uint index = static_cast<_uint>(type);

	if (index < FIXED_COMPONENT_COUNT)
	{
		if (nullptr != m_arrComponents[index])
			return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 원형 컴포넌트를 복제하여 사본 컴포넌트를 얻어오자. */
	CComponent* pComponent = pGameInstance->Clone_Component(this, iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	//pComponent->SetGameObject(this);

	if (index < FIXED_COMPONENT_COUNT)
		m_arrComponents[index] = pComponent;
	else
		m_vecScripts.push_back(dynamic_cast<CMonoBehaviour*>(pComponent));

	//Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Free()
{
	Super::Free();

	for (auto& iter : m_arrComponents)
		Safe_Release(iter);

	for (auto& iter : m_vecScripts)
		Safe_Release(iter);
	
	m_vecScripts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	RELEASE_INSTANCE(CGameInstance);
}
