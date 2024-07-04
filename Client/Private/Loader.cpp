#include "stdafx.h"
#include "..\Public\Loader.h"

#include <filesystem>
#include "GameInstance.h"
#include "BackGround.h"
//
#include "BasicTerrain.h"
#include "TempCube.h"
#include "CollisionTest.h"
#include "PlayerController.h"
#include "MonsterController.h"
#include "BossController.h"
#include "MonsterStats.h"
#include "MainCameraController.h"
#include "StateMachine.h"
#include "BehaviorTree.h"
#include "TestAIController.h"
#include "FlyingCamera.h"
#include "MainCamera.h"
#include "StaticTest.h"
#include "StaticBase.h"
#include "StaticScene.h"
#include "Arena.h"
#include "P_Strife.h"
#include "HellHound.h"
#include "HellBrute.h"
#include "Goblin.h"
#include "DemonCaster.h"
#include "Strife_GunL.h"
#include "Strife_GunR.h"
#include "Moloch.h"
#include "Moloch_Sword.h"
#include "TremorCrystal.h"
#include "Dagon.h"
#include "DagonWave.h"
#include "Strife_Ammo_Default.h"
#include "Strife_Ammo_Beam.h"
#include "Strife_Ammo_Nature.h"
#include "SkyBox.h"
#include "Particle.h"
#include "Particle_Rain.h"
#include "Particle_WaveSplash.h"
#include "Particle_Waterfall.h"
#include "ParticleController.h"
#include "Strife_MotionTrail.h"
#include "Moloch_MotionTrail.h"
#include "Moloch_SwordSlash.h"
#include "Shockwave.h"
#include "Lava.h"
#include "Water.h"
#include "Fire.h"
#include "Lightning.h"
#include "WaterLightning.h"
#include "Lightning_Spark.h"
#include "Bubble.h"
#include "Orb.h"
#include "Bolts.h"
#include "Wave_Ring.h"
#include "GeyserCrack.h"
#include "CrystalParticle.h"
#include "Sun.h"
#include "SphereSwirl.h"
#include "Waterfall_Foar.h"
#include "Ring_Flat_Wave.h"
#include "WaterShield.h"
#include "SwordTrail.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY ThreadEntry(void* pArg)
{
	/* DX가 사용하는 모든 컴 객체를 초기화한다.  */	
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;

	/* 새로운 스레드를 만들자. */
	/* 스레드 : 내 코드를 읽어주는 하나의 흐름? */
	/* 3 : 생성한 스레드가 호출해야하는 함수의 주소 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_Level_GamePlay();
		break;
	case LEVEL_GAMETOOL:
		hr = Loading_For_Level_GameTool();
		break;
	}

	if (FAILED(hr))
		return -1;	

	LeaveCriticalSection(&m_Critical_Section);

	return 0;	
}

HRESULT CLoader::Loading_For_Level_Logo()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_Logo();
	Loading_Scripts_For_Level_Logo();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");
	

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");
	Loading_GameObjects_For_Level_Logo();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_Logo()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Texture_BackGround */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/TitleMenu.png")))))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_Logo()
{
	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_Logo()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_BackGround */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_GamePlay()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_GamePlay();
	Loading_Scripts_For_Level_GamePlay();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	Loading_GameObjects_For_Level_GamePlay();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Texture_Strife_Ammo_Default*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Strife_Ammo_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Ammo/Strife_Ammo_Default_%d.png"), 4))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_Strife_Ammo_Static*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Strife_Ammo_Beam"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Ammo/Strife_Ammo_Beam_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Strife_Muzzle_Default*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Strife_Muzzle_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Ammo/Strife_Muzzle_%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FlameMask*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlameMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particles/FlameMask_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Water_Normal*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Water_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Noise/Water_normal%d.png"), 3))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_Raindrop*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Raindrop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particles/Raindrop.dds")))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_CloudSplash*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CloudSplash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particles/CloudSplash_%d.png"), 4))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_WaterfallSplash*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WaterfallSplash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particles/WaterfallSplash_%d.png"), 4))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Mask_Swipe*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask_Swipe"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/Mask_Swipe.png")))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Noise_RGB*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise_RGB"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Noise/Noise_RGB.png")))))
	//	return E_FAIL;

	/* For.Prototype_Component_Texture_SwordSwordTrail_Gradient*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail_Gradient"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/SwordTrail/SwordTrail_Gradient.png")))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_SwordSwordTrail_Mask*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/SwordTrail/SwordTrail_Mask.png")))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_Shockwave*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shockwave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/ScreenSpace/Shockwave.png")))))
		return E_FAIL;

	/* For.Prototype_Component_StateMachine */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_BehaviorTree */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_NavMeshAgent */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_NavMeshAgent"),
		CNavMeshAgent::Create(m_pDevice, m_pContext, TEXT("../Bin/LevelData/NavMesh/MainStageNavMesh.dat")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_PlayerController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PlayerController"),
		CPlayerController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_MonsterController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_MonsterController"),
		CMonsterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_BossController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BossController"),
		CBossController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_ParticleController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ParticleController"),
		CParticleController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_MonsterStats*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_MonsterStats"),
		CMonsterStats::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_MainCameraController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_MainCameraController"),
		CMainCameraController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_TestAIController*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_TestAIController"),
		CTestAIController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BasicTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TempCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TempCube"), CTempCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CollisionTest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CollisionTest"), CCollisionTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyingCamera"), CFlyingCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StaticTest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticTest"), CStaticTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_P_Strife */
  	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_Strife"), CP_Strife::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_HellHound */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HellHound"), CHellHound::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_HellBrute */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HellBrute"), CHellBrute::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Goblin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goblin"), CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DemonCaster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DemonCaster"), CDemonCaster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Strife_GunL */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_GunL"), CStrife_GunL::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Strife_GunR */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_GunR"), CStrife_GunR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Moloch */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moloch"), CMoloch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Moloch_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moloch_Sword"), CMoloch_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dagon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dagon"), CDagon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_DagonWave */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DagonWave"), CDagonWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Strife_Ammo_Default */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_Ammo_Default"), CStrife_Ammo_Default::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Strife_Ammo_Static */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_Ammo_Beam"), CStrife_Ammo_Beam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Strife_Ammo_Nature */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_Ammo_Nature"), CStrife_Ammo_Nature::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Strife_Ammo_Static */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_Ammo_Static"), CStrife_Ammo_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* For.Prototype_GameObject_Fire*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire"), CFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lightning */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning"), CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_WaterLightning */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterLightning"), CWaterLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle"), CParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Particle_Rain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rain"), CParticle_Rain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Particle_WaveSplash */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_WaveSplash"), CParticle_WaveSplash::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Particle_Waterfall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Waterfall"), CParticle_Waterfall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Strife_MotionTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Strife_MotionTrail"), CStrife_MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Moloch_MotionTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moloch_MotionTrail"), CMoloch_MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shockwave */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shockwave"), CShockwave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lava_East_B1 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lava_East_B1"), CLava::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Water */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Water"), CWater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TremorCrystal_A */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_A"), CTremorCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TremorCrystal_B */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_B"), CTremorCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bubble */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bubble"), CBubble::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Wave_Ring */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wave_Ring"), CWave_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Moloch_SwordSlash */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moloch_Sword_Slash"), CMoloch_SwordSlash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Arena */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Arena"), CArena::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_GeyserCrack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GeyserCrack"), CGeyserCrack::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Sun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sun"), CSun::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_SwordTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"), CSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Waterfall_Foar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Waterfall_Foar"), CWaterfall_Foar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TremorCrystal_ */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_G"), CCrystalParticle::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_H"), CCrystalParticle::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_I"), CCrystalParticle::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_L"), CCrystalParticle::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TremorCrystal_M"), CCrystalParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bolts */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bolts0"), CBolts::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bolts1"), CBolts::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bolts2"), CBolts::Create(m_pDevice, m_pContext))) ||
		FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bolts3"), CBolts::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lightning_Spark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning_Spark"), CLightning_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Orb */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Orb"), COrb::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SphereSwirl */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SphereSwirl"), CSphereSwirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Ring_Flat_Wave */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ring_Flat_Wave"), CRing_Flat_Wave::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_WaterShield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterShield"), CWaterShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_StaticScene */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticScene"), CStaticScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Static */
	wstring strStaticFilePath = TEXT("../Bin/Resources/Models/Static/");
	for (const auto& entry : filesystem::directory_iterator(strStaticFilePath))
	{
		const wstring& strFileName = entry.path().stem();

		if (strFileName == TEXT("CStaticScene"))
			continue;

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_") + strFileName, CStaticBase::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_GameTool()
{
	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	Loading_Components_For_Level_GameTool();
	Loading_Scripts_For_Level_GameTool();

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	Loading_GameObjects_For_Level_GameTool();

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Components_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Scripts_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_GameObjects_For_Level_GameTool()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BasicTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_GrassTerrain*/
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrassTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* For.Prototype_GameObject_SkyBox*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FlyingCamera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyingCamera"), CFlyingCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StaticTest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticTest"), CStaticTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Static */
	wstring strStaticFilePath = TEXT("../Bin/Resources/Models/Static/");
	for (const auto& entry : filesystem::directory_iterator(strStaticFilePath))
	{
		const wstring& strFileName = entry.path().stem();

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_") + strFileName, CStaticBase::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);	

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}