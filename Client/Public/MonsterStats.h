#pragma once
#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "RigidDynamic.h"
#include "Transform.h"
#include "NavMeshAgent.h"

BEGIN(Engine)

class CTransform;
class CNavMeshAgent;

END

BEGIN(Client)

class CMonsterStats : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagMonsterStat
	{
		_int	iMaxHP;
		_int	iDPS;
	}MONSTERSTAT;

private:
	CMonsterStats(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterStats(const CMonsterStats& rhs);
	virtual ~CMonsterStats() = default;

public:
	virtual HRESULT	Initialize(void* pArg) override;

public:
	_int	GetMaxHP()				{ return m_iMaxHP; }
	_int	GetHP()					{ return m_iHP; }
	_int	GetDPS()				{ return m_iDPS; }

	void	SetHP(_int iHP)			{ m_iHP	= iHP; }
	void	SetiDPS(_int iiDPS)		{ m_iDPS = iiDPS; }
	void	Damaged(_int iDamage)	{ m_iHP -= iDamage; }

private:
	_int	m_iMaxHP	= 0;
	_int	m_iHP		= 0;
	_int	m_iDPS		= 0;

public:
	static	CMonsterStats* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;

};

END