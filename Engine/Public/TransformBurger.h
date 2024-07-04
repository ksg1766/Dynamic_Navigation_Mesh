#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransformBurger :
    public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = { 0.f };
		_float		fRotationRadianPerSec = { 0.0f };
	}TRANSFORM_DESC;

private:
	CTransformBurger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransformBurger(const CTransformBurger& rhs);
	virtual ~CTransformBurger() = default;

public:
	Vec3 Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	Matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	Matrix Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	Matrix Get_WorldFloat4x4_TP() const {
		Matrix	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	Matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	void Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Fix_Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);

private:
	_float4x4			m_WorldMatrix;
	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotationRadianPerSec = { 0.0f };

public:
	static CTransformBurger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END