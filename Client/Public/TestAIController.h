#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "RigidDynamic.h"

BEGIN(Client)

class CTestAIController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CTestAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestAIController(const CTestAIController& rhs);
	virtual ~CTestAIController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

private:
	void	AutoMove(_float fTimeDelta);
	void	LimitAllAxisVelocity();

private:
	CRigidDynamic*	m_pRigidBody;

	Vec3			m_vMaxLinearSpeed;
	Vec3			m_vLinearSpeed;

	Vec3			m_vMaxAngularSpeed;
	Vec3			m_vAngularSpeed;

public:
	static	CTestAIController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END