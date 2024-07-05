#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMonoBehaviour : public CComponent
{
	using Super = CComponent;

protected:
	CMonoBehaviour(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonoBehaviour(const CMonoBehaviour& rhs);
	virtual ~CMonoBehaviour() = default;
	
public:
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	//virtual HRESULT FixedUpdate(const _float& fTimeDelta)	override;

	virtual void	DebugRender()				override;

protected:
	CGameInstance* m_pGameInstance = nullptr;

public:
	virtual void Free() override;
};

END