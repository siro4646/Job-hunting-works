#include "descriptor.h"
#include "../descriptorHeap/descriptorHeap.h"

namespace ym
{
	//----
	void Descriptor::Destroy()
	{
	}

	//----
	void Descriptor::Release()
	{
		if (pParentHeap_)
		{
			pParentHeap_->ReleaseDescriptor(this);
		}
	}

}	// namespace ym

//	EOF

