#pragma once

#include "Component.h"

/* 1. ȭ�鿡 �׷������ϴ� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* 2. �����ϰ� �ִ� ��ü���� ������(��ο���)�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
	using Super = CComponent;
public:
	/* RG_NONBLEND : ���� �׷����� Blend������Ʈ���� ���� ������ ���� �ݵ�� �������� �ֵ��� ���� �׷����Ѵ�. */
	/* RG_BLEND : �������ϰ� �׷����� ��ü�鵵 �ݵ�� �ָ��ִ� ����� �׸���. */
	enum RENDERGROUP { RG_PRIORITY, RG_NONLIGHT, RG_SHADOW, RG_SHADOW_INSTANCE, RG_NONBLEND_INSTANCE, RG_NONBLEND, RG_PARTICLE_INSTANCE, RG_WATER, RG_DISTORTION, RG_BLEND, RG_BLEND_INSTANCE, RG_UI, RG_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderObjects();

#ifdef _DEBUG
public:
	HRESULT Add_Debug(class CComponent* pDebug)
	{
		m_RenderDebug.push_back(pDebug);
		Safe_AddRef(pDebug);
		return S_OK;
	}
#endif

private:
	vector<CGameObject*>						m_RenderObjects[RG_END];
	map<InstanceID, class CVIBuffer_Instance*>	m_InstanceBuffers;

	class CTargetManager*						m_pTargetManager = { nullptr };
	class CLightManager*						m_pLightManager = { nullptr };
	class CGraphicDevice*						m_pGraphicDevice = nullptr;

private:
	class CVIBuffer_Rect*						m_pVIBuffer = { nullptr };
	class CShader*								m_pShader = { nullptr };
	class CShader*								m_pShaderPostProcess = { nullptr };
	ID3D11DepthStencilView*						m_pShadowDSV = nullptr;

	_float4x4									m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_int										m_iWaterCaptureCount = 6;
	_int										m_iMaxWaterCaptureCount = 1;

	_float4x4									m_LightView, m_LightProj;
	_float										m_fShadowTargetSizeRatio = 8.f;

#ifdef _DEBUG
private:
	vector<CComponent*>							m_RenderDebug;
	_bool										m_bTargetOnOff = false;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_NonBlend_Instance();
	HRESULT Render_Particle_Instance();

	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();

	HRESULT Render_Water();

	HRESULT Render_Shadow();
	HRESULT Render_GodRay();
	HRESULT Render_Blur();
	HRESULT Render_Distortion();
	HRESULT Render_PostProcess();

	HRESULT Render_Blend();
	HRESULT Render_Blend_Instance();
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

private:
	void	AddInstanceData(InstanceID instanceId, InstancingData& data);
	void	ClearInstanceData();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END