#include "stdafx.h"
#include "DissolveManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CDissolveManager);

CDissolveManager::CDissolveManager()
{
}

HRESULT CDissolveManager::Reserve_Manager(ID3D11Device* pDevice)
{
	wstring strDissolveFilePath = TEXT("../Bin/Resources/Textures/Effect/Noise/Noise0.png");
	_tchar	szDissolveFilePath[MAX_PATH] = TEXT("");
	wsprintf(szDissolveFilePath, strDissolveFilePath.c_str());
	if(FAILED(CreateWICTextureFromFile(pDevice, szDissolveFilePath, nullptr, &m_ppDissolveSRV)))
		return E_FAIL;

	m_pGameInstance = GET_INSTANCE(CGameInstance);

	return S_OK;
}

void CDissolveManager::Tick_Dissolve(const _float& fTimeDelta)
{
	for (auto iter = m_listDissolve.begin(); iter != m_listDissolve.end(); )
	{
		if (iter->IsRunning)
		{
			iter->fCurTime += fTimeDelta;

			if (iter->fPlayTime < iter->fCurTime)
			{
				iter->fCurTime = iter->fPlayTime;
				iter->IsRunning = false;
				m_pGameInstance->DeleteObject(iter->pDissolveObject);
			}

			_float fDissolveAmount = iter->fCurTime / iter->fPlayTime;
			iter->pDissolveObject->GetShader()->Bind_RawValue("g_fDissolveAmount", &fDissolveAmount, sizeof(_float));
			++iter;
		}
		else
		{
			m_listDissolve.erase(iter++);
		}
	}
}

void CDissolveManager::AddDissolve(CGameObject* pDissolveObject, _float fPlayTime)
{
	pDissolveObject->GetShader()->Bind_Resource("g_DissolveTexture", m_ppDissolveSRV);
	pDissolveObject->GetShader()->SetPassIndex(2);
	Safe_AddRef(m_ppDissolveSRV);

	DISSOLVE_DESC desc;
	desc.pDissolveObject = pDissolveObject;
	desc.fPlayTime = fPlayTime;
	desc.IsRunning = true;

	m_listDissolve.push_back(desc);
}

void CDissolveManager::Free()
{
	/*while (!m_listDissolve.empty())
	{
		m_listDissolve.pop_back();
	}*/
	m_listDissolve.clear();

	Safe_Release(m_ppDissolveSRV);
	RELEASE_INSTANCE(CGameInstance);
}
