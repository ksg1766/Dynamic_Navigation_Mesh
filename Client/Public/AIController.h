#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"

BEGIN(Client)

class CAIController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAIController(const CAIController& rhs);
	virtual ~CAIController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
	void	SetRadius(const _float fRadius);
	void	SetLinearSpeed(const Vec3& vLinearSpeed);
	void	AddWayPoint(const Vec3& vWayPoint) { m_vecWayPoints.push_back(vWayPoint); }

private:
	void	AutoMove(_float fTimeDelta);

private:
	CTransform*	m_pTransform = nullptr;

	_bool		m_isMoving = false;
	Vec3		m_vLinearSpeed;
	_float		m_fAgentRadius;

	vector<Vec3> m_vecWayPoints;
	_int		m_iCurrentWayIdx = 0;

private:
	// DebugDraw
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

public:
	static	CAIController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END