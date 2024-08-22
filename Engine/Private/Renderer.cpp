#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GraphicDevice.h"
#include "GameInstance.h"
#include "TargetManager.h"
#include "LightManager.h"
#include "LevelManager.h"
#include "Layer.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "PipeLine.h"
#include "Transform.h"
#include "Model.h"
#include "Shader.h"
#include "VIBuffer_Instance.h"
#include "StructuredBuffer.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext, ComponentType::Renderer)
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
	, m_pGraphicDevice(CGraphicDevice::GetInstance())
{
	Safe_AddRef(m_pLightManager);
	Safe_AddRef(m_pTargetManager);
	Safe_AddRef(m_pGraphicDevice);
}

HRESULT CRenderer::Initialize_Prototype()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	
	if (nullptr == m_pDevice)
		return E_FAIL;

#ifdef _DEBUG
	constexpr _float fTargetX = 144.f;
	constexpr _float fTargetY = 81.f;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse"), fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal"), fTargetX, 3.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Depth"), 3.f * fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Shade"), 3.f * fTargetX, 3.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Specular"), 5.f * fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;	

#endif
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	XMStoreFloat4x4(&m_LightView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LightProj, XMMatrixPerspectiveLH(XMConvertToRadians(45.f), (_float)ViewportDesc.Width / (_float)ViewportDesc.Height, 1.f, 2000.f));

	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RG_END)
		return S_OK;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderObjects()
{
	ClearInstanceData();

	if (FAILED(Render_Priority()))
		return S_OK;
	if (FAILED(Render_NonLight()))
		return S_OK;

	if (FAILED(Render_NonBlend()))
		return S_OK;
	if (FAILED(Render_NonBlend_Instance()))
		return S_OK;

	if (FAILED(Render_LightAcc()))
		return S_OK;

	if (FAILED(Render_Deferred()))
		return S_OK;

	if (FAILED(Render_Blend()))
		return S_OK;
	if (FAILED(Render_Blend_Instance()))
		return S_OK;

	if (FAILED(Render_UI()))
		return S_OK;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return S_OK;
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RG_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONLIGHT].clear();
	
	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_Instance()
{
	map<InstanceID, vector<CGameObject*>> cache;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND_INSTANCE])
	{
		if (nullptr == pGameObject->GetModel())
			continue;

		const _int iPassIndex = pGameObject->GetShader()->GetPassIndex();
		const _int instanceId = pGameObject->GetModel()->GetInstanceID();
		InstanceID ID(iPassIndex, instanceId);
		cache[ID].push_back(pGameObject);

		Safe_Release(pGameObject);
	}

	for (auto& mapIter : cache)
	{
		vector<CGameObject*>& vecInstances = mapIter.second;

		const InstanceID instanceId = mapIter.first;

		CGameObject*& pHead = vecInstances[0];

		for (_int i = 0; i < vecInstances.size(); i++)
		{
			CGameObject*& pGameObject = vecInstances[i];
			InstancingData data;
			data.matWorld = pGameObject->GetTransform()->WorldMatrix();

			AddInstanceData(instanceId, data);
		}

		if (pHead->GetModel()->IsAnimModel())
		{// INSTANCING
			InstancedTweenDesc* tweenDesc = new InstancedTweenDesc;
			for (_int i = 0; i < vecInstances.size(); i++)
			{
				CGameObject*& pGameObject = vecInstances[i];
				tweenDesc->tweens[i] = pGameObject->GetModel()->GetTweenDesc();
			}

			pHead->GetModel()->PushTweenData(*tweenDesc);

			Safe_Delete(tweenDesc);
		}

		pHead->RenderInstance();	// BindShaderResource 호출을 위함.
		CVIBuffer_Instance*& buffer = m_InstanceBuffers[instanceId];
		pHead->GetModel()->RenderInstancing(buffer);
	}

	m_RenderObjects[RG_NONBLEND_INSTANCE].clear();

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	const _float4x4& viewInverse = pPipeLine->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &viewInverse)))
		return E_FAIL;
	const _float4x4& projInverse = pPipeLine->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &projInverse)))
		return E_FAIL;
	const _float4& camPos = pPipeLine->Get_CamPosition_Float4();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &camPos, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Normal"), "g_NormalTarget")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTarget")))
		return E_FAIL;

	m_pLightManager->Render(m_pShader, m_pVIBuffer);

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTarget")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Shade"), "g_ShadeTarget")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTarget")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Specular"), "g_SpecularTarget")))
		return E_FAIL;

	m_pShader->SetPassIndex(2);
	if (FAILED(m_pShader->Begin()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend_Instance()
{
	for (auto& pGameObject : m_RenderObjects[RG_BLEND_INSTANCE])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderInstance();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND_INSTANCE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	CLevelManager* pInstance = GET_INSTANCE(CLevelManager);

	if (KEY_PRESSING(KEY::CTRL) && KEY_PRESSING(KEY::SHIFT) && KEY_DOWN(KEY::T))
		m_bTargetOnOff = !m_bTargetOnOff;

	if (m_bTargetOnOff)
	{
		/*for (auto& pDebugCom : m_RenderDebug)
		{
			pDebugCom->Render();
			Safe_Release(pDebugCom);
		}
		m_RenderDebug.clear();*/

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pTargetManager->Render(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Render(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Render(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CLevelManager);

	return S_OK;
}
#endif

void CRenderer::AddInstanceData(InstanceID instanceId, InstancingData& data)
{
	if (m_InstanceBuffers.find(instanceId) == m_InstanceBuffers.end())
		m_InstanceBuffers[instanceId] = CVIBuffer_Instance::Create(m_pDevice, m_pContext);

	m_InstanceBuffers[instanceId]->AddData(data);
}

void CRenderer::ClearInstanceData()
{
	for (auto& pair : m_InstanceBuffers)
	{
		CVIBuffer_Instance* pBuffer = pair.second;
		pBuffer->ClearData();
	}
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(CGameObject* pGameObject, void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	RELEASE_INSTANCE(CGraphicDevice);
	RELEASE_INSTANCE(CLightManager);
	RELEASE_INSTANCE(CTargetManager);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShadowDSV);

	ClearInstanceData();
	Super::Free();
}