#include "sampler.h"

#include "../device/device.h"
#include "../descriptor/descriptor.h"
#include "../descriptorHeap/descriptorHeap.h"

namespace ym
{
	//----
	bool Sampler::Initialize(Device *pDev, const D3D12_SAMPLER_DESC &desc)
	{
		descInfo_ = pDev->GetSamplerDescriptorHeap().Allocate();
		if (!descInfo_.IsValid())
		{
			return false;
		}

		pDev->GetDeviceDep()->CreateSampler(&desc, descInfo_.cpuHandle);

		samplerDesc_ = desc;

		return true;
	}

	//----
	void Sampler::Destroy()
	{
		descInfo_.Free();
	}

}	// namespace ym

//	EOF
