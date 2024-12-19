#pragma once
#include "application.h"

#include "../graphics/dx12/resource/base/resourceLoader.h"
#include "../graphics/dx12/resource/mesh/resourceMesh.h"
#include "../graphics/dx12/resource/texture/resourceTexture.h"
#include "../graphics/dx12/shaderManager/shaderManager.h"
#include "../graphics/dx12/commandList/commandList.h"
#include "../graphics/dx12/RootSignature/rootSignature.h"
#include "../graphics/dx12/texture/texture.h"
#include "../graphics/dx12/textureView/textureView.h"
#include "../graphics/dx12/buffer/buffer.h"
#include "../graphics/dx12/bufferView/bufferView.h"
#include "../graphics/dx12/commandQueue/commandQueue.h"
#include "../graphics/dx12/descriptor/descriptor.h"
#include "../graphics/dx12/descriptorHeap/descriptorHeap.h"
#include "../graphics/dx12/swapChain/swapChain.h"
#include "../utility/fileStream/fileStream.h"
#include "../graphics/dx12/shader/shader.h"
#include "../graphics/dx12/descriptorSet/descriptorSet.h"
#include "../graphics/dx12/fence/fence.h"
#include "../utility/resourceRelease/deatnList.h"
#include "../graphics/dx12/buffer/constantBuffer/constantBuffer.h"
#include "../graphics/dx12/mesh/meshManager/meshManager.h"
#include "../graphics/dx12/sceneRoot/sceneRoot.h"
#include "../graphics/dx12/sceneMesh/sceneMesh.h"
#include "../graphics/dx12/renderCommand/renderCommand.h"


namespace ym {
	class Game : public Application
	{
		struct Sphere
		{
			DirectX::XMFLOAT3	center;
			float				radius;
			DirectX::XMFLOAT4	color;
			uint32_t			material;

			Sphere()
				: center(0.0f, 0.0f, 0.0f), radius(1.0f), color(1.0f, 1.0f, 1.0f, 1.0f), material(0)
			{}
			Sphere(const DirectX::XMFLOAT3 &c, float r, const DirectX::XMFLOAT4 &col, uint32_t mat)
				: center(c), radius(r), color(col), material(mat)
			{}
			Sphere(float cx, float cy, float cz, float r, const DirectX::XMFLOAT4 &col, uint32_t mat)
				: center(cx, cy, cz), radius(r), color(col), material(mat)
			{}
		};
		struct Instance
		{
			DirectX::XMFLOAT4X4	mtxLocalToWorld;
			DirectX::XMFLOAT4	color;
			uint32_t			material;
		};

	public:
		Game(HINSTANCE hInstance, int nCmdShow, int screenWidth, int screenHeight, ColorSpaceType csType = ColorSpaceType::Rec709)
		:Application(hInstance, nCmdShow, screenWidth, screenHeight, csType)
		{

		}
		//virtual ~Game();

		// virtual
		 bool  Initialize()override;
		 bool  Execute() override;
		 void  Finalize()override;
		 int Input(UINT message, WPARAM wParam, LPARAM lParam)override;
	private:
		static const int kBufferCount = ym::Swapchain::kMaxBuffer;

		struct CommandLists
		{
			ym::CommandList	cmdLists[kBufferCount];
			int					index = 0;

			CommandLists()
			{}
			~CommandLists()
			{
				Destroy();
			}

			bool Initialize(ym::Device *pDev, ym::CommandQueue *pQueue)
			{
				for (auto &&v : cmdLists)
				{
					if (!v.Initialize(pDev, pQueue, true))
					{
						return false;
					}
				}
				index = 0;
				return true;
			}

			void Destroy()
			{
				for (auto &&v : cmdLists) v.Destroy();
			}

			ym::CommandList &Reset()
			{
				index = (index + 1) % kBufferCount;
				auto &&ret = cmdLists[index];
				ret.Reset();
				return ret;
			}

			void Close()
			{
				cmdLists[index].Close();
			}

			void Execute()
			{
				cmdLists[index].Execute();
			}

			ym::CommandQueue *GetParentQueue()
			{
				return cmdLists[index].GetParentQueue();
			}
		};	// struct CommandLists

		 //int  Input override(UINT message, WPARAM wParam, LPARAM lParam);

	};
}