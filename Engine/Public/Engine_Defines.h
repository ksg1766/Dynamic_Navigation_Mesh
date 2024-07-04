#pragma once

#pragma warning (disable : 4251)

// WIN
#include <windows.h>
#include <assert.h>
#include <optional>

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>

#include "SimpleMath.h"
#include <d3dcompiler.h>
#include <Effect11/d3dx11effect.h>
#include <DirectXColors.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>

//#include <FMOD/fmod.h>
//#include <FMOD/fmod.hpp>
//링커 입력 fmod_vc.libfmodL_vc.lib
#include <io.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace SimpleMath;

#include <memory>
#include <array>
#include <stack>
#include <iostream>
#include <vector> /* 배열 */
#include <list> /* 더블링크드리스트 */
#include <map> /* 이진트리 */
#include <unordered_map> /* 해시테이블 */
#include <queue>
#include <algorithm>
#include <functional>
#include <random>
//
#include <typeinfo>

#include <process.h>

#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

// Libs
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
//#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 

#endif

#endif // _DEBUG


using namespace Engine;



using namespace std;