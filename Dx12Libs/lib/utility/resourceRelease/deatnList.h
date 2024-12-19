#pragma once

#include "../utility.h"
#include <list>
#include "../../../Lib/graphics/dx12/swapChain/swapChain.h"


namespace ym
{
	struct PendingKillItem
	{
		int		pendingTime = 0;

		virtual ~PendingKillItem()
		{}

		bool CanKill()
		{
			pendingTime--;
			return pendingTime <= 0;
		}
	};	// struct PendingKillItem

	template <typename T>
	struct DeleteObjectItem
		: public PendingKillItem
	{
		T *pObject = nullptr;

		DeleteObjectItem(T *p)
			: pObject(p)
		{}
		~DeleteObjectItem()
		{
			SafeDelete(pObject);
		}
	};	// struct DeleteObjectItem

	template <typename T>
	struct ReleaseObjectItem
		: public PendingKillItem
	{
		T *pObject = nullptr;

		ReleaseObjectItem(T *p)
			: pObject(p)
		{}
		~ReleaseObjectItem()
		{
			SafeRelease(pObject);
		}
	};

	class DeathList
	{
	public:
		DeathList()
		{}
		~DeathList()
		{
			Destroy();
		}

		void Destroy()
		{
			auto it = items_.begin();
			while (it != items_.end())
			{
				auto p = *it;
				it = items_.erase(it);
				SafeDelete(p);
			}
		}

		void SyncKill()
		{
			auto it = items_.begin();
			while (it != items_.end())
			{
				auto p = *it;
				if (p->CanKill())
				{
					it = items_.erase(it);
					SafeDelete(p);
				}
				else
				{
					it++;
				}
			}
		}

		void PendingKill(PendingKillItem *p)
		{
			p->pendingTime = ym::Swapchain::kMaxBuffer;
			items_.push_back(p);
		}

		template <typename T>
		void KillObject(T *p)
		{
			auto item = new DeleteObjectItem<T>(p);
			PendingKill(item);
		}

	private:
		std::list<PendingKillItem *>		items_;
	};	// class DeathList

}	// namespace ym


//	EOF
