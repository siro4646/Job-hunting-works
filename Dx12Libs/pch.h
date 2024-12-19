#pragma once

//===============================================
// プリコンパイル済みヘッダー
//  ここに書いたものは初回のみ解析されるため、コンパイル時間が高速になる。
//  全てのcppからインクルードされる必要がある。
//===============================================
//===============================================
// 基本
//===============================================
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"winmm.lib")

#define NOMINMAX

#define _WINSOCK_DEPRECATED_NO_WARNINGS

//#define __builtin_FUNCSIG

#include <stdio.h>
#include <winsock2.h>
#include <winsock.h>
#include <conio.h>
#pragma comment (lib, "Ws2_32.lib")
#include <Windows.h>
#include <iostream>
#include <cassert>

#include <wrl/client.h>

//===============================================
// STL
//===============================================

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <fileSystem>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
using namespace std;
//===============================================
// Direct3D12
//===============================================
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler")
//#include <d3dx12.h>
// DirectX Tool Kit
//#pragma comment(lib, "DirectXTK12.lib")
//#include <SimpleMath.h>

#include <DirectXMath.h>
using namespace DirectX;

// DirectX Tex
#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

// Boost
#include <boost/asio.hpp>

//Assimp
#include <C:/assimp/include/assimp/cimport.h>
#include <C:/assimp/include/assimp/Importer.hpp>
#include <C:/assimp/include/assimp/scene.h>
#include <C:/assimp/include/assimp/postprocess.h>
#include <C:/assimp/include/assimp/matrix4x4.h>
#pragma comment (lib, "assimp-vc143-mt.lib")

//Direct2D
#include <d2d1.h>

//サードパーティー
//一度のみdefine STB_IMAGE_IMPLEMENTATIONを定義する
#include "thirdParty/cereal/cereal.hpp"
#include "thirdParty/cereal/access.hpp"
#include "thirdParty/cereal/archives/binary.hpp"
#include "thirdParty/cereal/types/string.hpp"
#include "thirdParty/cereal/types/vector.hpp"
//#include "thirdParty/imgui/imgui.h"



//自作系

//utility

#include "lib/utility/utility.h"
#include "lib/define/type/types.h"
#include "lib/utility/stringUtility.h"
