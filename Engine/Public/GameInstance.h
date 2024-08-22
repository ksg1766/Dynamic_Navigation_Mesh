#pragma once

#include "ComponentManager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT			Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext, _In_ HINSTANCE hInst);
	void			Tick(const _float& fTimeDelta);
	void			DebugRender();

	void			Clear(_uint iLevelIndex);

public: /* For.TimerManager */
	_float			Compute_TimeDelta(const wstring& strTimerTag);
	HRESULT			Add_Timer(const wstring& strTimerTag);

public: /* For.GraphicDevice */
	HRESULT			Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT			Clear_DepthStencil_View();
	Viewport&		GetViewPort();
	HRESULT			Present();

public: /* For.LevelManager */
	HRESULT			Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint			GetCurrentLevelIndex();
	void			SetCurrentLevelIndex(_uint iLevelIndex);

public: /* For.ObjectManager */
	HRESULT			Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	CGameObject*	Add_GameObject(_uint iLevelIndex, const LAYERTAG& eLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	map<LAYERTAG, class CLayer*>&	GetCurrentLevelLayers();

public: /* For.ComponentManager */
	HRESULT			Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(CGameObject* pGameObject, _uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.EventManager */
	void			FinalTick();
	CGameObject*	CreateObject(const wstring& strPrototypeTag, const LAYERTAG& eLayer, void* pArg = nullptr);
	void			DeleteObject(CGameObject* pObj);
	void			LevelChange(class CLevel* pLevel, _uint iLevelId);

public: /* For.CameraManager */
	HRESULT			AddCamera(const wstring& strName, CGameObject* pCamera);
	HRESULT			DeleteCamera(const wstring& strName);
	HRESULT			ChangeCamera(const wstring& strName);
	HRESULT			ChangeCamera();
	CGameObject*	GetCurrentCamera();

public: /* For.InputDevice */
	_bool			Key_Down(_ubyte byKeyID);
	_bool			Key_Pressing(_ubyte byKeyID);
	_bool			Key_Up(_ubyte byKeyID);

	_bool			Mouse_Down(MOUSEKEYSTATE eMouseKeyID);
	_bool			Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID);
	_bool			Mouse_Up(MOUSEKEYSTATE eMouseKeyID);

	_ubyte			Get_DIKeyState(_ubyte eKeyID);
	_ubyte			Get_DIMouseState(MOUSEKEYSTATE eMouseKeyID);
	_long			Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID);

	_bool			Get_AnyKeyDown();
	
public:	/* For.KeyManager */
	KEYSTATE		GetKeyState(KEY _eKey);
	const POINT&	GetMousePos();

public:	/* For.ShaderManager */
	HRESULT			SwapShader(CGameObject* pGameObject, const wstring& strShaderFileName);

public: /* For.PipeLine */
	HRESULT			Bind_TransformToShader(class CShader* pShader, const _char* pConstantName, CPipeLine::TRANSFORMSTATE eState);
	_float4			Get_CamPosition_Float4() const;
	_vector			Get_CamPosition_Vector() const;
	_float4x4		Get_Transform_float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix			Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4		Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix			Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;

public: /* For.LightManager */
	LIGHT_DESC*		Get_LightDesc(_uint iLightIndex);
	HRESULT			Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT			Add_Sun(CGameObject* pSun);

public: /* For.SoundManager */
	HRESULT			PlaySoundFile(const wstring& strSoundKey, CHANNELID eCh, _float fVolume);
	HRESULT			CheckPlaySoundFile(const wstring& strSoundKey, CHANNELID eCh, _float fVolume);
	HRESULT			PlayBGM(const wstring& strSoundKey, _float fVolume);
	HRESULT			StopSound(CHANNELID eCh);
	HRESULT			StopSoundAll();
	HRESULT			SetChannelVolume(CHANNELID eCh, _float fVolume);

private:
	class CTimerManager*			m_pTimerManager = { nullptr };
	class CGraphicDevice*			m_pGraphicDevice = { nullptr };
	class CInputDevice*				m_pInputDevice = { nullptr };
	class CKeyManager*				m_pKeyManager = { nullptr };
	class CLevelManager*			m_pLevelManager = { nullptr };
	class CObjectManager*			m_pObjectManager = { nullptr };
	class CComponentManager*		m_pComponentManager = { nullptr };
	class CCameraManager*			m_pCameraManager = { nullptr };
	class CEventManager*			m_pEventManager = { nullptr };
	class CShaderManager*			m_pShaderManager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLightManager*			m_pLightManager = { nullptr };
	class CTargetManager*			m_pTargetManager = { nullptr };
	class CSoundManager*			m_pSoundManager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END