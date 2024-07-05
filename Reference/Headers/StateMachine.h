#pragma once
#include "MonoBehaviour.h"
#include "Hasher.h"

BEGIN(Engine)

class CState;
class ENGINE_DLL CStateMachine final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;
	
public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;

	virtual void	DebugRender()				override;

public:
	CState* GetCurrState() const { return m_pCurrState; }

	HRESULT ChangeState(const wstring& strState, _int iAnimIndex = 0);
	HRESULT AddState(CState* pState);

	HRESULT SetController(CMonoBehaviour* pController);

private:
	using STATES = unordered_map<wstring, CState*, djb2Hasher>;
	STATES	m_hashStates;

	CState* m_pCurrState = nullptr;

	CMonoBehaviour* m_pController = nullptr;

public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free() override;
};

END