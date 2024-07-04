#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Water.h"
#include "FileUtils.h"
#include "Utils.h"
#include "Cell.h"
#include "Particle_Waterfall.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_pGameInstance = GET_INSTANCE(CGameInstance);

	m_pGameInstance->StopSoundAll();

	//if (FAILED(LoadData_Map()))
	if (FAILED(LoadData_Map_Area1()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Default()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Ground()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UnitGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UnitAir()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Equipment()))
		return E_FAIL;

	if (FAILED(Ready_Layer_IgnoreCollision()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Wall()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Build_QuadTree(LEVELID::LEVEL_GAMEPLAY)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->PlayBGM(TEXT("mus_hell_med_drums.ogg"), 0.3f)))
		__debugbreak();

	return S_OK;
}

HRESULT CLevel_GamePlay::Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Update_QuadTree();

	return S_OK;
}

HRESULT CLevel_GamePlay::LateTick(const _float& fTimeDelta)
{
	//SetWindowText(g_hWnd, TEXT("�����÷��� �����Դϴ�."));
	
	// TODO: �� ���� ���� �� ���� ��ġ�� ã���� ����....
	m_pGameInstance->LateTick_Collision(fTimeDelta);

	m_pGameInstance->Render_QuadTree(fTimeDelta);
#ifdef _DEBUG
#endif // !DEBUG


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc;

	/* ���⼺ ������ �߰��ϳ�. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	//LightDesc.vLightDir = _float4(1.f, -1.5f, 1.f, 0.f);
	//LightDesc.vLightDir = _float4(-0.53f, -0.43f, -0.72f, 0.f);
	//LightDesc.vLightDir = _float4(-0.745f, -0.445f, 0.45f, 0.f);
	LightDesc.vLightDir = _float4(-0.45f, -0.445f, -0.745f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	XMStoreFloat4(&LightDesc.vSpecular, XMLoadFloat4(&LightDesc.vDiffuse));

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	/* �� ������ �߰��Ѵ�. */
	/*ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vLightPos = _float4(35.f, 3.f, 35.f, 1.f);
	LightDesc.fLightRange = 20.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Default()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::DEFAULT;

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_SkyBox"));
	if (nullptr == pGameObject)	return E_FAIL; 

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Sun"));
	m_pGameInstance->Add_Sun(pGameObject);
	if (nullptr == pGameObject)	return E_FAIL; 

	/*pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_SkyPlane"));
	if (nullptr == pGameObject)	return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::TERRAIN;

	//CWater::WATER_DESC tWaterDesc = { Vec3(0.f, 28.25f, 265.f), Vec2(1280.f, 500.f) };
	CWater::WATER_DESC tWaterDesc = { Vec3(0.f, 0.f, 265.f), Vec2(1280.f, 500.f) };
	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Water"), &tWaterDesc);
	if (nullptr == pGameObject)	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Ground()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::GROUND;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::PLAYER;

	/*pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_TempCube"));
	if (nullptr == pGameObject)	return E_FAIL;*/
	
	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_P_Strife"));
	if (nullptr == pGameObject)	return E_FAIL;
	//pGameObject->GetTransform()->Translate(Vec3(0.f, 0.f, 50.f));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Equipment()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::EQUIPMENT;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_IgnoreCollision()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::IGNORECOLLISION;

	for (_int i = 0; i < 5; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Particle_Waterfall"));
		if (nullptr == pGameObject)	return E_FAIL;
		static_cast<CParticle_Waterfall*>(pGameObject)->SetEmitDirection(Vec3(0.f, 0.f, -1.f));
		pGameObject->GetTransform()->Translate(Vec3(-154.5f + 4.0f * i, 48.f, 251.5f));
	}

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Particle_Waterfall"));
	if (nullptr == pGameObject)	return E_FAIL;
	static_cast<CParticle_Waterfall*>(pGameObject)->SetEmitDirection(Vec3(0.205f, 0.f, -1.f));
	pGameObject->GetTransform()->Translate(Vec3(-170.f, 45.5f, 245.f));

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Particle_Waterfall"));
	if (nullptr == pGameObject)	return E_FAIL;
	static_cast<CParticle_Waterfall*>(pGameObject)->SetEmitDirection(Vec3(-0.215f, 0.f, -1.f));
	pGameObject->GetTransform()->Translate(Vec3(-124.15f, 45.5f, 245.f));

	for (_int i = 0; i < 5; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Particle_Waterfall"));
		if (nullptr == pGameObject)	return E_FAIL;
		static_cast<CParticle_Waterfall*>(pGameObject)->SetEmitDirection(Vec3(0.f, 1.f, -0.05f));
		pGameObject->GetTransform()->Translate(0.33f * Vec3::UnitY + Vec3(-154.5f + 4.0f * i, 29.f, 242.7f));
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UnitGround()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::UNIT_GROUND;

	/*pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
	if (nullptr == pGameObject)	return E_FAIL;
	pGameObject->GetNavMeshAgent()->SetCurrentIndex(34);

	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(80);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(81);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(82);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(83);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(84);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(85);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(86);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(87);
	}

	for (_int i = 0; i < 5; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(124 + 4 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(125 + 4 * i);
	}

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
	if (nullptr == pGameObject)	return E_FAIL;
	pGameObject->GetNavMeshAgent()->SetCurrentIndex(135);

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
	if (nullptr == pGameObject)	return E_FAIL;
	pGameObject->GetNavMeshAgent()->SetCurrentIndex(139);

	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(162);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(164);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(165);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(166);
	}

	for (_int i = 0; i < 6; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(246 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(251 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(250 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(248 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(247 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(249 + 6 * i);
	}

	for (_int i = 0; i < 11; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(284 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(285 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(286 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(287 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(288 + 6 * i);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(289 + 6 * i);
	}

	for (_int i = 0; i < 9; ++i)
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_DemonCaster"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(401 + i);
	}
	*/
	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(574);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(579);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(580);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(581);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(582);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(583);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(584);
	}

	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(604);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(606);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(607);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(608);
	}

	{
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(635);
		
		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(636);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_Goblin"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(637);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(638);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellHound"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(639);

		pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_HellBrute"));
		if (nullptr == pGameObject)	return E_FAIL;
		pGameObject->GetNavMeshAgent()->SetCurrentIndex(640);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UnitAir()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::CAMERA;

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_MainCamera"));
	if (nullptr == pGameObject)	return E_FAIL;

	pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayerTag, TEXT("Prototype_GameObject_FlyingCamera"));
	if (nullptr == pGameObject)	return E_FAIL;
	pGameObject->GetTransform()->Translate(Vec3(0.f, 400.f, 0.f));

	m_pGameInstance->ChangeCamera(TEXT("MainCamera"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Wall()
{
	CGameObject* pGameObject = nullptr;
	LAYERTAG	eLayerTag = LAYERTAG::WALL;

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadData_Map()
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/MainStage_v04.dat"), Read);
	//file->Open(TEXT("../Bin/LevelData/NavTest.dat"), Read);

	while (true)
	{
		// key �� ����
		LAYERTAG eLayer = LAYERTAG::LAYER_END;
		string strLayerTag = "";

		if (false == file->Read(strLayerTag))
			break;

		const _int LayerTag_End = sizeof(LayerTag_string) / sizeof(_char*);
		for (_int i = 0; i < LayerTag_End; ++i)
		{
			if (0 == strcmp(LayerTag_string[i], strLayerTag.c_str()))
			{
				eLayer = static_cast<LAYERTAG>(i);
				break;
			}
		}
		if (LAYERTAG::LAYER_END == eLayer)
			__debugbreak();

		Matrix matWorld = file->Read<Matrix>();
		string tempObjectTag;
		file->Read(tempObjectTag);

		if (tempObjectTag == "Water_Pond" || tempObjectTag == "WaterFall_Foar")
			continue;

		if (static_cast<_uint>(LAYERTAG::DEFAULT_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer))
		{

		}

		if (static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::STATIC_LAYER_END) > static_cast<_uint>(eLayer))
		{
			const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + Utils::ToWString(tempObjectTag);

			//CGameObject* pGameObject = m_pGameInstance->CreateObject(strPrototypeTag, eLayer);
			CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayer, strPrototypeTag);
			if (nullptr == pGameObject)
				__debugbreak();
			pGameObject->GetTransform()->Set_WorldMatrix(matWorld);
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadData_Map_Area0()
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/MainStage_v04.dat"), Read);
	//file->Open(TEXT("../Bin/LevelData/NavTest.dat"), Read);

	while (true)
	{
		// key �� ����
		LAYERTAG eLayer = LAYERTAG::LAYER_END;
		string strLayerTag = "";

		if (false == file->Read(strLayerTag))
			break;

		const _int LayerTag_End = sizeof(LayerTag_string) / sizeof(_char*);
		for (_int i = 0; i < LayerTag_End; ++i)
		{
			if (0 == strcmp(LayerTag_string[i], strLayerTag.c_str()))
			{
				eLayer = static_cast<LAYERTAG>(i);
				break;
			}
		}
		if (LAYERTAG::LAYER_END == eLayer)
			__debugbreak();

		Matrix matWorld = file->Read<Matrix>();
		
		if (0 <= matWorld.Translation().x)
		{
			string tempObjectTag;
			file->Read(tempObjectTag);	// Pass~

			continue;
		}

		string tempObjectTag;
		file->Read(tempObjectTag);

		if (tempObjectTag == "Water_Pond")
			continue;

		if (static_cast<_uint>(LAYERTAG::DEFAULT_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer))
		{

		}

		if (static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::STATIC_LAYER_END) > static_cast<_uint>(eLayer))
		{
			const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + Utils::ToWString(tempObjectTag);

			//CGameObject* pGameObject = m_pGameInstance->CreateObject(strPrototypeTag, eLayer);
			CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayer, strPrototypeTag);
			if (nullptr == pGameObject)
				__debugbreak();
			pGameObject->GetTransform()->Set_WorldMatrix(matWorld);
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadData_Map_Area1()
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(TEXT("../Bin/LevelData/MainStage_v04.dat"), Read);
	//file->Open(TEXT("../Bin/LevelData/NavTest.dat"), Read);

	while (true)
	{
		// key �� ����
		LAYERTAG eLayer = LAYERTAG::LAYER_END;
		string strLayerTag = "";

		if (false == file->Read(strLayerTag))
			break;

		const _int LayerTag_End = sizeof(LayerTag_string) / sizeof(_char*);
		for (_int i = 0; i < LayerTag_End; ++i)
		{
			if (0 == strcmp(LayerTag_string[i], strLayerTag.c_str()))
			{
				eLayer = static_cast<LAYERTAG>(i);
				break;
			}
		}
		if (LAYERTAG::LAYER_END == eLayer)
			__debugbreak();

		Matrix matWorld = file->Read<Matrix>();
		if (-50.f > matWorld.Translation().x)
		{
			string tempObjectTag;
			file->Read(tempObjectTag);	// Pass~

			continue;
		}
		
		string tempObjectTag;
		file->Read(tempObjectTag);

		if (tempObjectTag == "Water_Pond")
			continue;

		if (static_cast<_uint>(LAYERTAG::DEFAULT_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer))
		{

		}

		if (static_cast<_uint>(LAYERTAG::DYNAMIC_LAYER_END) < static_cast<_uint>(eLayer) &&
			static_cast<_uint>(LAYERTAG::STATIC_LAYER_END) > static_cast<_uint>(eLayer))
		{
			const wstring strPrototypeTag = TEXT("Prototype_GameObject_") + Utils::ToWString(tempObjectTag);

			//CGameObject* pGameObject = m_pGameInstance->CreateObject(strPrototypeTag, eLayer);
			CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, eLayer, strPrototypeTag);
			if (nullptr == pGameObject)
				__debugbreak();
			pGameObject->GetTransform()->Set_WorldMatrix(matWorld);
		}
	}

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	Super::Free();
	CModel::ReleaseAllVTF();

	RELEASE_INSTANCE(CGameInstance);
}
