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
#include "VIBuffer_Point.h"
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

	/* For.Target_Emissive */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Emissive"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	///* For.Target_DepthBlue */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_DepthBlue"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Scene */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Scene"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Priority */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Priority"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_DepthShadow */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"),
		m_fShadowTargetSizeRatio * ViewportDesc.Width, m_fShadowTargetSizeRatio * ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.f))))
		return E_FAIL;

	/* For.Target_BlendScene */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlendScene"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Distortion */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Distortion"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	
	/* For.Target_NonBlendFinal */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_NonBlendFinal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_BlendFinal */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlendFinal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (nullptr == m_pDevice)
		return E_FAIL;

	// Todo: 나중에 옮기자ㅎㅎ
#pragma region DSV
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_fShadowTargetSizeRatio * ViewportDesc.Width;
	TextureDesc.Height = m_fShadowTargetSizeRatio * ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


#pragma endregion DSV

	/* For.Target_Distortion */
	/*if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Distortion"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;*/

#ifdef _DEBUG
	constexpr _float fTargetX = 144.f;
	constexpr _float fTargetY = 81.f;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse"), fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal"), fTargetX, 3.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Depth"), 3.f * fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Emissive"), 5.f * fTargetX, 3.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Shade"), 3.f * fTargetX, 3.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Specular"), 5.f * fTargetX, fTargetY, 288.f, 162.f)))
		return E_FAIL;
	//
	//
	/*if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Distortion"), 144.f, 567.f, 288.f, 162.f)))
		return E_FAIL;*/
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_DepthBlue"), 3.f * fTargetX, 5.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_ShadowDepth"), 5.f * fTargetX, 5.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_BlendScene"), 5.f * fTargetX, 7.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Distortion"), 5.f * fTargetX, 9.f * fTargetY, 288.f, 162.f)))
		return E_FAIL;

#endif

	/* 이 렌더타겟들은 그려지는 객체로부터 값을 저장받는다. */
	/* For.MRT_GameObjects */
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Priority"), TEXT("Target_Priority"))))
		return E_FAIL;
	
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_DepthBlue"))))
		return E_FAIL;

	/* 이 렌더타겟들은 게임내에 존재하는 빛으로부터 연산한 결과를 저장받는다. */
	/* For.MRT_Lights */
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Blend"), TEXT("Target_BlendScene"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Blend"), TEXT("Target_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_NonBlendFinal"), TEXT("Target_NonBlendFinal"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_BlendFinal"), TEXT("Target_BlendFinal"))))
		return E_FAIL;

	/*if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;*/

	/*if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_PostProcess"), TEXT("Target_Distortion"))))
		return E_FAIL;*/

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	
	m_pShaderPostProcess = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderPostProcess)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	XMStoreFloat4x4(&m_LightView, XMMatrixIdentity());
	//Matrix m;
	//XMStoreFloat4x4(&m_LightProj, XMMatrixOrthographicLH((_float)ViewportDesc.Width * 0.15f, (_float)ViewportDesc.Height * 0.15f, 1.f, 2000.f));
	XMStoreFloat4x4(&m_LightProj, XMMatrixPerspectiveLH(XMConvertToRadians(45.f), (_float)ViewportDesc.Width / (_float)ViewportDesc.Height, 1.f, 2000.f));
	//m_LightProj._44 = 0.f;

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

	if (FAILED(Render_Shadow()))
		return S_OK;

	ClearInstanceData();

	/*if (FAILED(Render_Water()))
		return S_OK;*/

	{
		if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
			return E_FAIL;

		if (FAILED(Render_NonBlend()))
			return S_OK;
		if (FAILED(Render_NonBlend_Instance()))
			return S_OK;

		//
		/*for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
			Safe_Release(pGameObject);
		for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
			Safe_Release(pGameObject);
		for (auto& pGameObject : m_RenderObjects[RG_NONBLEND_INSTANCE])
			Safe_Release(pGameObject);

		m_RenderObjects[RG_PRIORITY].clear();
		m_RenderObjects[RG_NONBLEND].clear();
		m_RenderObjects[RG_NONBLEND_INSTANCE].clear();*/

		if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
			return E_FAIL;
		//
	}

	//if (FAILED(Render_Particle_Instance()))
	//	return S_OK;
	if (FAILED(Render_LightAcc()))
		return S_OK;

	if (FAILED(Render_Deferred()))
		return S_OK;
	
	//if (FAILED(Render_GodRay()))
	//	return S_OK;

	//if (FAILED(Render_Blur()))
	//	return S_OK;

	//if (FAILED(Render_PostProcess()))
	//	return S_OK;

	if (FAILED(Render_Blend()))
		return S_OK;
	if (FAILED(Render_Blend_Instance()))
		return S_OK;

	//if (FAILED(Render_PostProcess()))
	//	return S_OK;

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
	//if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Priority"))))
	//	return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	//if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
	//	return E_FAIL;

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
				tweenDesc->tweens[i] = pGameObject->GetModel()->GetTweenDesc();	// 소켓 아이템의 경우 어떻게 할지.(굳이 인스턴싱이 필요 없을 듯 함 근데.)
			}

			pHead->GetModel()->PushTweenData(*tweenDesc);

			Safe_Delete(tweenDesc);
		}
		else
		{
			for (auto& iter : vecInstances)
			{
				iter->InitRendered();
			}
		}

		pHead->RenderInstance();	// BindShaderResource 호출을 위함.
		CVIBuffer_Instance*& buffer = m_InstanceBuffers[instanceId];
		pHead->GetModel()->RenderInstancing(buffer);
	}

	m_RenderObjects[RG_NONBLEND_INSTANCE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Particle_Instance()
{
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Effect"))))
		return E_FAIL;

	map<InstanceID, vector<CGameObject*>> cache;

	for (auto& pGameObject : m_RenderObjects[RG_PARTICLE_INSTANCE])
	{
		if (nullptr == pGameObject->GetBuffer())
			continue;

		const _int iPassIndex = pGameObject->GetShader()->GetPassIndex();
		const _int instanceId = static_cast<CVIBuffer_Point*>(pGameObject->GetBuffer())->GetInstanceID();
		InstanceID ID(iPassIndex, instanceId);

		if (cache[ID].size() < 500)
			cache[ID].push_back(pGameObject);

		Safe_Release(pGameObject);
	}

	for (auto& mapIter : cache)
	{
		vector<CGameObject*>& vecInstances = mapIter.second;

		const InstanceID instanceId = mapIter.first;

		CGameObject*& pHead = vecInstances[0];
		CVIBuffer_Point* pPointBuffer = static_cast<CVIBuffer_Point*>(pHead->GetBuffer());

		for (_int i = 0; i < vecInstances.size(); i++)
		{
			CGameObject*& pGameObject = vecInstances[i];
			InstancingData data;
			data.matWorld = pGameObject->GetTransform()->WorldMatrix();

			AddInstanceData(instanceId, data);
		}

		//////////
		/*
		CShader* pShader = static_cast<CShader*>(CComponentManager::GetInstance()
			->Clone_Component(m_pGameObject, 0, TEXT("Prototype_Component_Shader_ComputeParticles"), nullptr));
		//Safe_AddRef(pShader);

		vector<InstancingData>& vecInstanceData = m_InstanceBuffers[instanceId]->GetInstanceData();
		_int iDataSize = vecInstanceData.size();
		CStructuredBuffer* pStructuredBuffer = CStructuredBuffer::Create(m_pDevice, m_pContext, vecInstanceData.data(), sizeof(InstancingData), iDataSize, sizeof(InstancingData), iDataSize);
		//Safe_AddRef(pStructuredBuffer);

		pShader->Bind_Texture("Input", pStructuredBuffer->GetSRV());	// Texture지만 BindResource라고 생각하자
		pShader->Get_UAV("Output", pStructuredBuffer->GetUAV());
		
		// Input
		pShader->Dispatch(0, 1, 1, 1);

		// Output
		//vector<InstancingData> vecInstanceDataOut(iDataSize);
		if (SUCCEEDED(pStructuredBuffer->CopyFromOutput(vecInstanceData.data())))
		{
			for (_int i = 0; i < vecInstances.size(); ++i)
			{
				vecInstances[i]->GetTransform()->Set_WorldMatrix(vecInstanceData[i].matWorld);
			}
		}
		*/
		//////////
		pHead->RenderInstance();	// BindShaderResource 호출을 위함.
		CVIBuffer_Instance*& buffer = m_InstanceBuffers[instanceId];
		buffer->Render(pPointBuffer);

		//Safe_Release(pShader);
		//Safe_Release(pStructuredBuffer);
	}

	m_RenderObjects[RG_PARTICLE_INSTANCE].clear();

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* 빛연산의 결과를 그려놓을 Shade타겟을 장치에 바인딩 한다. */
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	/* 사각형 버퍼를 직교투영으로 Shade타겟의 사이즈만큼 꽉 채워서 그릴꺼야. */
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

	/* 다시 장치의 0번째 소켓에 백 버퍼를 올린다. */
	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Shadow"), m_pShadowDSV)))
		return E_FAIL;
	m_pContext->ClearDepthStencilView(m_pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	CObjectManager* pObjectManager = GET_INSTANCE(CObjectManager);
	map<LAYERTAG, class CLayer*>& pLayers = pObjectManager->GetCurrentLevelLayers();
	RELEASE_INSTANCE(CObjectManager);
	const auto& iter = pLayers.find(LAYERTAG::PLAYER);
	if (pLayers.end() == iter || nullptr == iter->second)
	{
		if (FAILED(m_pTargetManager->End_MRT(m_pContext, m_pShadowDSV)))
			return E_FAIL;
		return S_OK;
	}

	CGameObject* pPlayer = iter->second->GetGameObjects().front();

	const Vec3& vPos = pPlayer->GetTransform()->GetPosition();
	_vector vPosition = XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f);

	const LIGHT_DESC* pLightDesc = m_pLightManager->Get_LightDesc(0);
	const _float4& vLightDir = pLightDesc->vLightDir;
	_vector vLightPosition = XMVectorSet(vPos.x - 180.f * vLightDir.x, vPos.y - 180.f * vLightDir.y, vPos.z - 180.f * vLightDir.z, 1.f);

	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_float fOriginalWidth = ViewportDesc.Width;
	_float fOriginalHeight = ViewportDesc.Height;

	ViewportDesc.Width *= m_fShadowTargetSizeRatio;
	ViewportDesc.Height *= m_fShadowTargetSizeRatio;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);
	XMStoreFloat4x4(&m_LightView, XMMatrixLookAtLH(vLightPosition, vPosition, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	for (auto& pGameObject : m_RenderObjects[RG_SHADOW])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->RenderShadow(m_LightView, m_LightProj);
		}

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOW].clear();

	/////////////////////////////////////

	map<InstanceID, vector<CGameObject*>> cache;	// 요거 데이터 영역에 두면 중복 호출 피할 수 있음.

	for (auto& pGameObject : m_RenderObjects[RG_SHADOW_INSTANCE])
	{
		if (nullptr == pGameObject->GetModel())
			continue;

		const _int iPassIndex = pGameObject->GetShader()->GetPassIndex();
		const _int instanceId = pGameObject->GetModel()->GetInstanceID();
		InstanceID ID(iPassIndex, instanceId);
		cache[ID].push_back(pGameObject);

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOW_INSTANCE].clear();

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
			InstancedTweenDesc* tweenDesc = new InstancedTweenDesc;	// 요거 데이터 영역에 두면 중복 호출 피할 수 있음2.
			for (_int i = 0; i < vecInstances.size(); i++)
			{
				CGameObject*& pGameObject = vecInstances[i];
				tweenDesc->tweens[i] = pGameObject->GetModel()->GetTweenDesc();	// 소켓 아이템의 경우 어떻게 할지.(굳이 인스턴싱이 필요 없을 듯 함 근데.)
			}

			pHead->GetModel()->PushTweenData(*tweenDesc);

			Safe_Delete(tweenDesc);
		}
		else
		{
			for (auto& iter : vecInstances)
			{
				iter->InitRendered();
			}
		}

		_int iOriginalPass = pHead->GetShader()->GetPassIndex();

		pHead->RenderShadow(m_LightView, m_LightProj);	// BindShaderResource 호출을 위함.
		CVIBuffer_Instance*& buffer = m_InstanceBuffers[instanceId];
		pHead->GetModel()->RenderShadowInstancing(buffer);

		pHead->GetShader()->SetPassIndex(iOriginalPass);	// 구조상 바로 안돌려놓으면 패스아이디가 달라져서 다른 InstanceID가 생성될 수도 있음...
	}

	/////////////////////////////////////

	if (FAILED(m_pTargetManager->End_MRT(m_pContext, m_pShadowDSV)))
		return E_FAIL;
	/*if (FAILED(m_pGraphicDevice->Clear_DepthStencil_View()))
		return E_FAIL;*/

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	ViewportDesc.Width = fOriginalWidth;
	ViewportDesc.Height = fOriginalHeight;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGraphicDevice->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	//if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Scene"))))
	//	return E_FAIL;

	/* 디퓨즈 타겟과 셰이드 타겟을 서로 곱하여 백버퍼에 최종적으로 찍어낸다. */
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

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_DepthBlue"), "g_BlueTarget")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTarget")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_ShadowDepth"), "g_ShadowDepthTarget")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Priority"), "g_PriorityTarget")))
		return E_FAIL;
	
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Specular"), "g_SpecularTarget")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &m_LightView)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &m_LightProj)))
		return E_FAIL;
	
	//////////
	static _float fBias = 0.0001f;

	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::C))
		fBias += 0.0001f;

	if (KEY_PRESSING(KEY::CTRL) && KEY_DOWN(KEY::V))
		fBias -= 0.0001f;

	if (FAILED(m_pShader->Bind_RawValue("g_fBias", &fBias, sizeof(_float))))
		return E_FAIL;
	//////////

	m_pShader->SetPassIndex(3);
	if (FAILED(m_pShader->Begin()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	//if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_DISTORTION].clear();

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	// NonBlend & Blend Scene 합치기
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_NonBlendFinal"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_Scene"), "g_SceneTarget")))
		return E_FAIL;


	m_pShader->SetPassIndex(4);
	if (FAILED(m_pShader->Begin()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_BlendFinal"))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_NonBlendFinal"), "g_SceneTarget")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShader, TEXT("Target_BlendScene"), "g_BlendTarget")))
		return E_FAIL;

	m_pShader->SetPassIndex(8);
	if (FAILED(m_pShader->Begin()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	/////////////////////
	// PostProcess

	if (FAILED(m_pShaderPostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderPostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderPostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_SRV(m_pShaderPostProcess, TEXT("Target_BlendFinal"), "g_SceneTarget")))
		return E_FAIL;
	
	if (FAILED(m_pTargetManager->Bind_SRV(m_pShaderPostProcess, TEXT("Target_Distortion"), "g_DistortionTarget")))
		return E_FAIL;

	m_pShaderPostProcess->SetPassIndex(0);
	if (FAILED(m_pShaderPostProcess->Begin()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	//if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Blend"))))
	//	return E_FAIL;

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

	//if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
	//	return E_FAIL;

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
		/*if (FAILED(m_pTargetManager->Render(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer)))
			return E_FAIL;*/
		/*if (FAILED(m_pTargetManager->Render(TEXT("MRT_Scene"), m_pShader, m_pVIBuffer)))
			return E_FAIL;*/
		/*if (FAILED(m_pTargetManager->Render(TEXT("MRT_BlurHorizontal"), m_pShader, m_pVIBuffer)))
			return E_FAIL;*/
		//if (FAILED(m_pTargetManager->Render(TEXT("MRT_BlurVertical"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//if (FAILED(m_pTargetManager->Render(TEXT("MRT_Refraction_Final"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//if (FAILED(m_pTargetManager->Render(TEXT("MRT_Reflection_Final"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		if (FAILED(m_pTargetManager->Render(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
			return E_FAIL;
		//if (FAILED(m_pTargetManager->Render(TEXT("MRT_GodRay"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		//if (FAILED(m_pTargetManager->Render(TEXT("MRT_Blend"), m_pShader, m_pVIBuffer)))
		//	return E_FAIL;
		/*if (FAILED(m_pTargetManager->Render(TEXT("MRT_PostProcess"), m_pShader, m_pVIBuffer)))
			return E_FAIL;*/
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