#include "ShaderManager.h"
#include "ComponentManager.h"
#include "LevelManager.h"
#include "GameObject.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CShaderManager);

CShaderManager::CShaderManager()
{
}

HRESULT CShaderManager::SwapShader(CGameObject* pGameObject, const wstring& strShaderFileName)
{
	CComponentManager* pComponentManager = GET_INSTANCE(CComponentManager);
	CLevelManager* pLevelManager = GET_INSTANCE(CLevelManager);

	CComponent* pDelete = pGameObject->m_arrComponents[(_uint)ComponentType::Shader];

	wstring strShaderProtoTag = TEXT("Prototype_Component_") + strShaderFileName;
	CShader* pNew = static_cast<CShader*>(pComponentManager->Clone_Component(pGameObject, 0, strShaderProtoTag));
	
	if (nullptr == pNew)
	{
		RELEASE_INSTANCE(CComponentManager);
		RELEASE_INSTANCE(CLevelManager);
		return E_FAIL;
	}

	pGameObject->m_arrComponents[(_uint)ComponentType::Shader] = pNew;
	if (pGameObject->GetModel())
		pGameObject->GetModel()->SetShader(pNew);
	
	Safe_Release(pDelete);

	RELEASE_INSTANCE(CComponentManager);
	RELEASE_INSTANCE(CLevelManager);

	return S_OK;
}

void CShaderManager::Free()
{
	Super::Free();
}
