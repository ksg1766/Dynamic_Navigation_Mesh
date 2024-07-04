#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "RigidDynamic.h"
#include "Transform.h"
#include "NavMeshAgent.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CParticleController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	enum class ParticleType { FLY, FLOAT, EXPLODE, RIGIDBODY, TYPE_END};
	typedef struct tagParticleDesc
	{
		ParticleType	eType = ParticleType::TYPE_END;

		_float3			vCenter = _float3(0.f, 0.f, 0.f);
		_float3			vRange = _float3(2.f, 1.f, 2.f);
		_float3			vSpeedMin = _float3(-3.2f, 2.5f, -3.2f), vSpeedMax = _float3(3.2f, 3.5f, 3.2f);
		_float			fScaleMin = 0.1f, fScaleMax = 0.2f;
		_float			fLifeTimeMin = 1.f, fLifeTimeMax = 2.f;
		//Color			vColor = Color(1.f, 1.f, 0.1f, 1.f);
		_int			iPass = 0;
	}PARTICLE_DESC;

private:
	CParticleController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleController(const CParticleController& rhs);
	virtual ~CParticleController() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;

private:
	void	RandomFly(const _float& fTimeDelta);
	void	RandomFloat(const _float& fTimeDelta);
	void	RandomExplode(const _float& fTimeDelta);
	void	RigidExplode(const _float& fTimeDelta);

private:
	CTransform*		m_pTransform	= nullptr;
	CRigidDynamic*	m_pRigidBody	= nullptr;

	Vec3			m_vSpeed;
	_float			m_fLifeTIme		= 0.f;

	PARTICLE_DESC	m_tProps;

public:
	static	CParticleController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END