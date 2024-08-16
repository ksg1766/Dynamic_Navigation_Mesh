#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "Cell.h"

BEGIN(Engine)

class CTransform;
class CTerrain;
struct Cell;
struct Obst;

END

BEGIN(Client)

class CAgentController : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CAgentController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAgentController(const CAgentController& rhs);
	virtual ~CAgentController() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()				override;

public:
	_bool	IsIdle();
	_bool	IsMoving();

	_bool	Pick(CTerrain* pTerrain, _uint screenX, _uint screenY);

public:
	void	SetRadius(const _float fRadius);
	void	SetLinearSpeed(const Vec3& vLinearSpeed);

private:
	void	Input(_float fTimeDelta);

private:
	CTransform*		m_pTransform = nullptr;

	_bool			m_isMoving = false;
	Vec3			m_vLinearSpeed;
	_float			m_fAgentRadius;

	// DebugDraw
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect*	m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

public:
	static	CAgentController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END