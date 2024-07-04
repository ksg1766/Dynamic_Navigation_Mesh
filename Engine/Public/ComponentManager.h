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
#include "RigidDynamic.h"
#include "RigidStatic.h"
#include "ColliderAABB.h"
#include "ColliderOBB.h"
#include "ColliderSphere.h"
#include "ColliderCylinder.h"
#include "MonoBehaviour.h"

#include "VIBuffer_Point.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Grid.h"
#include "VIBuffer_Instance.h"
#include "VIBuffer_Dot.h"
#include "VIBuffer_ParticleSystem.h"

/* ������Ʈ���� ������ �����Ѵ�. */
/* �纻��? ���� ������Ʈ�� ����ϰ����ϴ� ��ü���� ���� �����Ѵ�. */

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
	/* ������ü���� �������� �����ұ�?! */
	unordered_map<const wstring, class CComponent*, djb2Hasher>*	m_pPrototypes = { nullptr };
	typedef unordered_map<const wstring, class CComponent*, djb2Hasher>	PROTOTYPES;

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	virtual void Free() override;
};

END