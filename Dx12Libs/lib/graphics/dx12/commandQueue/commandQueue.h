#pragma once

//ToDo : 動作検証、コメント追加



namespace ym
{
	class Device;

	class CommandQueue
	{
		friend class CommandList;

	public:
		CommandQueue()
		{}
		~CommandQueue()
		{
			Destroy();
		}

		bool Initialize(Device *pDev, D3D12_COMMAND_LIST_TYPE type, u32 priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);
		void Destroy();

		uint64_t GetTimestampFrequency() const;

		ID3D12CommandQueue *GetQueueDep() { return pQueue_; }

	private:
		ID3D12CommandQueue *pQueue_{ nullptr };
		D3D12_COMMAND_LIST_TYPE listType_{ D3D12_COMMAND_LIST_TYPE_DIRECT };
	};	// class CommandQueue

}	// namespace ym

//	EOF
