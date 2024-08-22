#pragma once

#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Terrain.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Model.h"
#include "NavMeshAgent.h"
#include "MonoBehaviour.h"

#include "VIBuffer_Sphere.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Grid.h"
#include "VIBuffer_Instance.h"

/* 컴포넌트들의 원형을 보관한다. */
/* 사본은? 실제 컴포넌트를 사용하고자하는 객체들이 각각 보관한다. */

BEGIN(Engine)

class CComponentManager final : public CBase
{
	DECLARE_SINGLETON(CComponentManager)

private:
	CComponentManager();
	virtual ~CComponentManager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(CGameObject* pGameObject, _uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

private:
	_uint											m_iNumLevels = { 0 };
	/* 원형객체들을 레벨별로 보관할까?! */
	map<const wstring, class CComponent*>*	m_pPrototypes = { nullptr };
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	virtual void Free() override;
};

END