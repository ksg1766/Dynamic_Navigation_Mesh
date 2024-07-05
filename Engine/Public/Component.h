#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CGameInstance;
class CTransform;

enum class ComponentType
{
	RigidBody,
	Transform,
	Collider,
	Buffer,
	Model,
	Shader,
	Renderer,
	Texture,
	Camera,
	//Animator,
	Light,
	Terrain,
	NavMeshAgent,
	// ...
	Script,

	End,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<_uint>(ComponentType::End) - 1
};

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ComponentType type);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg)		PURE;
	virtual void	Tick(_float fTimeDelta)		{};
	virtual void	LateTick(_float fTimeDelta)	{};
	//virtual HRESULT FixedUpdate(_float fTimeDelta)	PURE;

	virtual void	DebugRender()				{};

public:
	ComponentType	GetType() const { return m_eType; }

	CGameObject*	GetGameObject() const;
	CTransform*		GetTransform() const;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_isCloned = { false };

protected:
	ComponentType			m_eType;
	CGameObject*			m_pGameObject;
	CGameInstance*			m_pGameInstance;

private:
	friend class CGameObject;
	void SetGameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; }

public:
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) = 0;
	virtual void Free() override;
};

END