#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight;
class CGameObject;
class CShader;
class CVIBuffer_Rect;
class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
private:
	CLightManager();
	virtual ~CLightManager() = default;

public:
	LIGHT_DESC* Get_LightDesc(_uint iLightIndex = 0);
	_float3 Get_SunScreenPos();

public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer);

	Matrix GetLightViewMatrix(_uint iLightIndex = 0);
	Matrix GetLightProjMatrix(_uint iLightIndex = 0);

	HRESULT Add_Sun(CGameObject* pSun);

private:
	vector<CLight*>	m_vecLights;
	CGameObject*	m_pSun = nullptr;

public:
	virtual void Free() override;
};

END