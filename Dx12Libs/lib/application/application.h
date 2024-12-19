#pragma once

#define NOMINMAX
#include "../graphics/dx12/device/device.h"
#include "../graphics/dx12/sampler/sampler.h"


namespace ym
{

	/*********************************************************//**
	 * @brief アプリケーションクラス
	*************************************************************/
	class Application
	{
	public:
		Application(HINSTANCE hInstance, int nCmdShow, int screenWidth, int screenHeight, ColorSpaceType csType = ColorSpaceType::Rec709);
		virtual ~Application();

		int Run();

		// virtual
		virtual bool Initialize() = 0;
		virtual bool Execute() = 0;
		virtual void Finalize() = 0;
		virtual int Input(UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

	private:

	protected:
		HINSTANCE		hInstance_ = 0;
		HWND			hWnd_ = 0;
		int				screenWidth_ = 0, screenHeight_ = 0;

		ym::CpuTimer	deltaTime_;

		ym::Device	device_;

		ym::Sampler	pointWrapSampler_;
		ym::Sampler	pointClampSampler_;
		ym::Sampler	linearWrapSampler_;
		ym::Sampler	linearClampSampler_;
	};	// class Application

}	// namespace ym

//	EOF
