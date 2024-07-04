#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CWater final : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagWaterDesc
	{
		_float3 vPos;
		_float2 vSize;
	}WATER_DESC;

	enum class WaterLevelMode { Dagon, Dessert, End };

private:
	/* 원형을 생성할 때 */
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CWater(const CWater& rhs); /* 복사 생성자. */
	virtual ~CWater() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

public:
	void			SetMode(const WaterLevelMode& eMode) { m_eMode = eMode; }

private:
	void			Dagon(const _float& fTimeDelta);
	void			Desert(const _float& fTimeDelta);

private:
	HRESULT			Ready_FixedComponents(WATER_DESC* pDesc);
	HRESULT			Ready_Scripts();
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

	_float			m_fWaterTranslation = 0.1f;
	_float			m_fReflectRefractScale = 0.1f;

	_int			m_iCurrentNormalMap = 1;
	_float			m_fWaterTranslationSpeed = 0.015f;

	WaterLevelMode	m_eMode = WaterLevelMode::End;

	_bool			m_bFresnel = true;

public:
	static	CWater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END