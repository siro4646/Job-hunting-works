#include "commandQueue.h"
#include "../device/device.h"

//ToDo : 動作検証、コメント追加

namespace ym
{
	//----
	bool CommandQueue::Initialize(Device *pDev, D3D12_COMMAND_LIST_TYPE type, u32 priority)
	{
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Type = type;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		// GPUタイムアウトが有効
		desc.Priority = priority;
		auto hr = pDev->GetDeviceDep()->CreateCommandQueue(&desc, IID_PPV_ARGS(&pQueue_));
		if (FAILED(hr))
		{
			return false;
		}

		listType_ = type;
		return true;
	}
	//----
	void CommandQueue::Destroy()
	{
		SafeRelease(pQueue_);
	}

	//----
	uint64_t CommandQueue::GetTimestampFrequency() const
	{
		if (!pQueue_)
		{
			return 1;
		}

		uint64_t ret;
		auto hr = pQueue_->GetTimestampFrequency(&ret);
		if (FAILED(hr))
		{
			return 1;
		}
		return ret;
	}
}
