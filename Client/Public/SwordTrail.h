#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CVIBuffer_Point;

END

BEGIN(Client)

class CSwordTrail final : public CGameObject
{
public:
	typedef struct tagCSwordTrailDesc
	{
		CModel*		pModel = nullptr;
		//_float	fLifeTime = 0.3f;

		Matrix		matOffsetTop;
		_int		iIndex = 0;

	}SWORDTRAIL_DESC;

	using Super = CGameObject;
private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(const _float& fTimeDelta)		override;
	virtual void	LateTick(const _float& fTimeDelta)	override;
	virtual void	DebugRender()						override;
	virtual HRESULT Render()							override;

public:

private:
	CShader*		m_pShaderStream = nullptr;
	CModel*			m_pModel		= nullptr;

	CTexture*		m_pMaskTexture	= nullptr;

	_float			m_fLifeTime		= 0.3f;
	_float			m_fTimeDelta	= 0.0f;

	//TWEENDESC		m_tTweenDesc;
	_int			m_iBoneIndex	= 0;
	_int			m_iTextureIndex	= 0;

	Matrix			m_matOffsetTop;
	Matrix*			m_pOffsetBottom = nullptr;

	//Vec2			m_vDistortionOffset = Vec2(0.f, 0.f);

private:
	HRESULT			Bind_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */
	HRESULT			Ready_FixedComponents();
	HRESULT			Ready_Scripts();

public:
	static	CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END