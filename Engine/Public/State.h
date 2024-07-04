#pragma once
#include "Base.h"
#include "Hasher.h"

BEGIN(Engine)

class CGameInstance;
class CGameObject;
class CModel;
class CMonoBehaviour;
class CNavMeshAgent;
class ENGINE_DLL CState : public CBase
{
	using Super = CBase;
public:
	typedef struct tagStateAnimations
	{	// cache
		vector<wstring>	vecAnimations;
	}STATEANIMS;

protected:
	CState();
	CState(const CState& rhs);
	virtual ~CState() = default;
	
public:
	virtual HRESULT			Initialize(CGameObject* pGameObject, const STATEANIMS& tStateAnim, CMonoBehaviour* pController = nullptr);
	virtual HRESULT			Enter(_int = 0);

	virtual void			Tick(const _float& fTimeDelta)		PURE;
	virtual const wstring&	LateTick(const _float& fTimeDelta)	PURE;

	virtual void			Exit()								PURE;
	virtual const wstring&	Transition()						PURE;
	
	const wstring&			GetName()							{ return m_strStateName; };

protected:
	CGameObject*	m_pGameObject = nullptr;
	CGameInstance*	m_pGameInstance = nullptr;
	CMonoBehaviour* m_pController = nullptr;

	wstring			m_strStateName = TEXT("");
	vector<wstring>	m_vecTransition;

	CModel*			m_pModel = nullptr;
	_int			m_iCurrAnimation;
	vector<pair<_int, _float>> m_vecAnimIndexTime;
	_float			m_fTimeSum = 0.f;


public:
	virtual void Free() override;
};

END