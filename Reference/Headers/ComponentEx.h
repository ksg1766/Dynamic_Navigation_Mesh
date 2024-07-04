#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

enum class ComponentType : uint8
{
	Transform,
	//MeshRenderer,
	//ModelRenderer,
	Renderer,
	Camera,
	Animator,
	Light,
	RigidBody,
	Collider,
	Terrain,
	// ...
	Script,

	End,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1
};

class ENGINE_DLL CComponent abstract
	: public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ComponentType type);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype()				PURE;
	virtual HRESULT Initialize(void* pArg)				PURE;	// Start

	virtual void	Tick(_float fTimeDelta)				PURE;
	virtual void	LateTick(_float fTimeDelta)			PURE;
	//virtual HRESULT FixedUpdate(_float fTimeDelta)	PURE;

	virtual void	DebugRender()						PURE;

public:
	ComponentType GetType() { return m_eType; }

	CGameObject* GetGameObject();
	CTransform* GetTransform();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	ComponentType m_eType;
	CGameObject* m_pGameObject;

private:
	friend class CGameObject;
	void SetGameObject(CGameObject* _gameObject) { m_pGameObject = _gameObject; }

public:
	virtual CComponent* Clone(void* pArg) PURE;	// Awake
	virtual void Free() override;
};

END