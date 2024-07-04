#include "..\Public\LightManager.h"
#include "Light.h"
#include "GameObject.h"
#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLightManager)

CLightManager::CLightManager()
{

}

LIGHT_DESC* CLightManager::Get_LightDesc(_uint iLightIndex)
{
	if (iLightIndex >= m_vecLights.size())
		return nullptr;

	return m_vecLights[iLightIndex]->Get_LightDesc();
}

_float3 CLightManager::Get_SunScreenPos()
{
	if (!m_pSun)
		return _float3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);

	const Vec3& vSunPosWorld = m_pSun->GetTransform()->GetPosition();

	CPipeLine* pPipeline = GET_INSTANCE(CPipeLine);

	Vec3 vSunPosProj(vSunPosWorld.x, vSunPosWorld.y, vSunPosWorld.z);

	vSunPosProj = XMVector3TransformCoord(XMVector3TransformCoord(vSunPosWorld, pPipeline->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW)),
		pPipeline->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	vSunPosProj.x = (vSunPosProj.x + 1.f) * 0.5f;
	vSunPosProj.y = (vSunPosProj.y - 1.f) * -0.5f;

	return vSunPosProj;

}

HRESULT CLightManager::Add_Light(const LIGHT_DESC & LightDesc)
{
	CLight*		pLight = CLight::Create(LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_vecLights.push_back(pLight);

	return S_OK;
}

HRESULT CLightManager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_vecLights)
	{
		pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

Matrix CLightManager::GetLightViewMatrix(_uint iLightIndex)
{
	const LIGHT_DESC* tLightDesc = m_vecLights[iLightIndex]->Get_LightDesc();
	return XMMatrixLookAtLH(XMLoadFloat4(&tLightDesc->vLightPos), XMLoadFloat4(&tLightDesc->vLightDir), XMVectorSet(0.f, 1.f, 0.f, 0.f));
}

Matrix CLightManager::GetLightProjMatrix(_uint iLightIndex)
{
	const LIGHT_DESC* tLightDesc = m_vecLights[iLightIndex]->Get_LightDesc();
	return XMMatrixPerspectiveLH(XMConvertToRadians(45.f), 1440.f / 810.f, 1.f, 2000.f);
}

HRESULT CLightManager::Add_Sun(CGameObject* pSun)
{
	if(nullptr != m_pSun)
		return E_FAIL;

	m_pSun = pSun;
}

void CLightManager::Free()
{
	for (auto& pLight : m_vecLights)
		Safe_Release(pLight);

	m_vecLights.clear();
}
