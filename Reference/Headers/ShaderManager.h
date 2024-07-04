#pragma once
#include "Base.h"

BEGIN(Engine)

class CShader;
class CGameObject;
class CShaderManager final : public CBase
{
	using Super = CBase;
    DECLARE_SINGLETON(CShaderManager);

private:
	CShaderManager();
	virtual ~CShaderManager() = default;

public:
	HRESULT	SwapShader(CGameObject* pGameObject, const wstring& strShaderFileName);

public:
	virtual void Free() override;
};

END