#pragma once
#include "MonoBehaviour.h"
#include "Hasher.h"

BEGIN(Engine)

struct tagBlackBoard	// Interface
{
	virtual void*	GetValue() PURE;
};

template<typename T>
struct tagBlackBoardData : public tagBlackBoard
{
public:
	tagBlackBoardData() = default;
	tagBlackBoardData(T _value) :value(_value) {}
	virtual void*	GetValue() override	{ return &value; }
	void			SetValue(T _value)	{ value = _value; }

private:
	T value;
};

template<typename T>
struct tagBlackBoardData<T*> : public tagBlackBoard
{
public:
	tagBlackBoardData() = default;
	tagBlackBoardData(T* _value) :value(_value) {}
	virtual void*	GetValue() override { return value; } // 이거 캐스팅 안되면 value 바로 가져오도록
	void			SetValue(T* _value) { value = _value; }

private:
	T* value;
};

#define GET_VALUE(type, var) static_cast<type*>(var->second->GetValue())
#define SET_VALUE(type, var, value) static_cast<tagBlackBoardData<type>*>(var->second)->SetValue(value)

using BLACKBOARD = unordered_map<wstring, tagBlackBoard*, djb2Hasher>;

class CBT_Node;
class CBT_Composite;
class ENGINE_DLL CBehaviorTree final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
protected:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;
	
public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()						override;

	void			SetRoot(CBT_Composite* pComposite)	{ m_pRootNode = pComposite; }
	
	BLACKBOARD&		GetBlackBoard()						{ return m_BlackBoard; }

protected:
	CBT_Composite*		m_pRootNode = nullptr;
	
	BLACKBOARD			m_BlackBoard;

public:
	static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free() override;
};

END