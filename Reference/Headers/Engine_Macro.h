#include <format>

#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
	}();

#ifndef			MSG
#define			MSG(_message)			MessageBox(nullptr, _message, L"System Message", MB_OK)
#endif

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			ASSERT_LOG()			{ const std::string message{ std::format("Error! \n\nFunction Name : {} \n\nLien Number : {}", __FUNCTION__, __LINE__ - 1) };\
											MSG(Engine::Utils::ToWString(message).c_str()); } // 사용시  #include "Utils_String.h" 필수

#define			LOG(_message) { cout << _message; }

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)		using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__asm { int 3 };return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__asm { int 3 };return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__asm { int 3 };}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__asm { int 3 };return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __asm { int 3 };return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __asm { int 3 };return _return;}



#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void )		\
		{														\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

/* Color */
#define D3DCOLOR_ARGB(a,b,g,r) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define D3DCOLOR_RGBA(r,g,b,a) \
    ((D3DCOLOR)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

#define	D3D11_APPEND_ALIGNED_ELEMENT	( 0xffffffff )

// For.KeyManager
#define KEY_CHECK(key, state) m_pGameInstance->GetKeyState(key) == state
#define KEY_PRESSING(key) KEY_CHECK(key, KEYSTATE::HOLD)
#define KEY_DOWN(key) KEY_CHECK(key, KEYSTATE::TAP)
#define KEY_UP(key) KEY_CHECK(key, KEYSTATE::AWAY)
#define KEY_NONE(key) KEY_CHECK(key, KEYSTATE::NONE)

#define KEY_CHECK_EX(key, state) CGameInstance::GetInstance()->GetKeyState(key) == state
#define KEY_PRESSING_EX(key) KEY_CHECK_EX(key, KEYSTATE::HOLD)
#define KEY_DOWN_EX(key) KEY_CHECK_EX(key, KEYSTATE::TAP)
#define KEY_UP_EX(key) KEY_CHECK_EX(key, KEYSTATE::AWAY)
#define KEY_NONE_EX(key) KEY_CHECK_EX(key, KEYSTATE::NONE)

#define MOUSE_DOWN(key) m_pGameInstance->Mouse_Down(key)
#define MOUSE_PRESSING(key) m_pGameInstance->Mouse_Pressing(key)
#define MOUSE_UP(key) m_pGameInstance->Mouse_Up(key)
#define MOUSE_NONE(key)	if(!MOUSE_DOWN(key) && !MOUSE_PRESSING(key) && !MOUSE_UP(key))	\
							{ return true; }											\
						else															\
							{ return false; }

#define MOUSE_DOWN_EX(key) CGameInstance::GetInstance()->Mouse_Down(key)
#define MOUSE_PRESSING_EX(key) CGameInstance::GetInstance()->Mouse_Pressing(key)
#define MOUSE_UP_EX(key) CGameInstance::GetInstance()->Mouse_Up(key)
#define MOUSE_NONE_EX(key)	if(!MOUSE_DOWN_EX(key) && !MOUSE_PRESSING_EX(key) && !MOUSE_UP_EX(key))	\
								{ return true; }													\
							else																	\
								{ return false; }

#endif // Engine_Macro_h__
