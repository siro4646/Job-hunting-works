#include "resourceManager.h"
#include "../commandList/commandList.h"
#include "../swapChain/swapChain.h"

namespace ym
{
	//-------------------------------------------
	// �����_�����O���\�[�X�̏�����
	//-------------------------------------------
	bool RenderResource::Initialize(ym::Device &device, const RenderResourceDesc &desc, ym::u32 screenWidth, ym::u32 screenHeight, D3D12_RESOURCE_STATES initialState)
	{
		// �~�b�v���x���͖�������K�v������
		if (desc.mipLevels == 0)
		{
			return false;
		}
		// RTV�ADSV�ASRV�AUAV�̓~�b�v���x���ɑ΂��ēK�؂Ȑ��l�łȂ���΂Ȃ�Ȃ�
		if ((desc.mipLevels < desc.targetCount)
			|| (desc.mipLevels + 1 < desc.srvCount)
			|| (desc.mipLevels < desc.uavCount))
		{
			return false;
		}

		desc_ = desc;

		// ���ƍ��������肷��
		ym::u32 width = desc.width;
		ym::u32 height = desc.height;
		if (desc.resolution_rate > 0.0f)
		{
			width = static_cast<ym::u32>(static_cast<float>(screenWidth) * desc.resolution_rate);
			height = static_cast<ym::u32>(static_cast<float>(screenHeight) * desc.resolution_rate);
			desc_.width = desc_.height = 0;
		}

		// RTV��DSV�������肷��
		bool isRtv = desc.targetCount > 0;
		bool isDsv = false;
		if (isRtv)
		{
			if ((desc.format == DXGI_FORMAT_D16_UNORM)
				|| (desc.format == DXGI_FORMAT_D24_UNORM_S8_UINT)
				|| (desc.format == DXGI_FORMAT_D32_FLOAT)
				|| (desc.format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT))
			{
				isRtv = false;
				isDsv = true;
			}
		}

		// �e�N�X�`���I�u�W�F�N�g����
		{
			ym::TextureDesc td{};
			td.dimension = ym::TextureDimension::Texture2D;
			td.width = width;
			td.height = height;
			td.depth = 1;
			td.format = desc.format;
			td.mipLevels = desc.mipLevels;
			td.sampleCount = desc.sampleCount;
			td.initialState = initialState;
			td.clearDepth = 1.0f;
			td.isRenderTarget = isRtv;
			td.isDepthBuffer = isDsv;
			td.isUav = desc.uavCount > 0;
			if (!texture_.Initialize(&device, td))
			{
				return false;
			}
		}

		// RTV����
		if (isRtv)
		{
			rtvs_.resize(desc.targetCount);
			for (ym::u32 i = 0; i < desc.targetCount; ++i)
			{
				if (!rtvs_[i].Initialize(&device, &texture_, i))
				{
					return false;
				}
			}
		}
		// DSV����
		else if (isDsv)
		{
			dsvs_.resize(desc.targetCount);
			for (ym::u32 i = 0; i < desc.targetCount; ++i)
			{
				if (!dsvs_[i].Initialize(&device, &texture_, i))
				{
					return false;
				}
			}
		}

		// SRV����
		if (desc.srvCount > 0)
		{
			srvs_.resize(desc.srvCount);

			// 0�Ԃ̓t���~�b�v���x��
			if (!srvs_[0].Initialize(&device, &texture_))
			{
				return false;
			}

			// 1�Ԉȍ~�̓~�b�v���x������
			for (ym::u32 i = 1; i < desc.srvCount; ++i)
			{
				if (!srvs_[i].Initialize(&device, &texture_, i - 1, 1))
				{
					return false;
				}
			}
		}

		// UAV����
		if (desc.uavCount)
		{
			uavs_.resize(desc.uavCount);
			for (ym::u32 i = 0; i < desc.uavCount; ++i)
			{
				if (!uavs_[i].Initialize(&device, &texture_, i))
				{
					return false;
				}
			}
		}

		state_ = initialState;

		return true;
	}

	//-------------------------------------------
	// �����_�����O���\�[�X�̉��
	//-------------------------------------------
	void RenderResource::Destroy()
	{
		for (auto &&v : rtvs_) v.Destroy();
		for (auto &&v : dsvs_) v.Destroy();
		for (auto &&v : srvs_) v.Destroy();
		for (auto &&v : uavs_) v.Destroy();
		texture_.Destroy();
	}


	//-------------------------------------------
	// �����_�����O���\�[�X�}�l�[�W���̏�����
	//-------------------------------------------
	bool RenderResourceManager::Initialize(ym::Device &device, ym::u32 screenWidth, ym::u32 screenHeight)
	{
		if (screenWidth == 0 || screenHeight == 0)
		{
			return false;
		}

		pDevice_ = &device;
		screenWidth_ = screenWidth;
		screenHeight_ = screenHeight;

		return true;
	}

	//-------------------------------------------
	// �����_�����O���\�[�X�}�l�[�W���̉��
	//-------------------------------------------
	void RenderResourceManager::Destroy()
	{
		AllReset();
		pDevice_ = nullptr;
	}

	//-------------------------------------------
	// �S���\�[�X���Z�b�g
	//-------------------------------------------
	void RenderResourceManager::AllReset()
	{
		for (auto &&v : resources_)
		{
			delete v;
		}
		resources_.clear();
		resource_map_.clear();
	}

	namespace
	{
		typedef std::map<ResourceID, ym::u32>		LastAccessPassInfo;

		void MakeResourceHistory(std::vector<ResourceProducerBase *> &producers, LastAccessPassInfo &lastAccess)
		{
			auto prodCnt = producers.size();
			ym::u16 passNo = 0;
			ResourceProducerBase *prev_prod = nullptr;
			for (auto &&prod : producers)
			{
				// �o�̓��\�[�X����������
				auto cnt = prod->GetOutputCount();
				auto ids = prod->GetOutputIds();
				for (ym::u16 i = 0; i < cnt; ++i)
				{
					// ID�̉��H���s��
					// ����p�r�������Ȃ������̏o�̓o�b�t�@(kPrevOutputID)�̏ꍇ�A�p�X�ԍ��Əo�͔ԍ����烆�j�[�N��ID�𐶐�����
					auto id = ids[i];
					if (id.isPrevOutput)
					{
						id = ResourceID::CreatePrevOutputID((ym::u8)passNo, (ym::u8)i);
						prod->SetOutputID(i, id);		// ID���Z�b�g������
					}

					lastAccess[id] = passNo;
				}

				// ���̓��\�[�X����������
				cnt = prod->GetInputCount();
				ids = prod->GetInputIds();
				for (ym::u16 i = 0; i < cnt; ++i)
				{
					// ID�̉��H���s��
					// kPrevOutputID�ȏ�̏ꍇ�͑O��p�X�̏o�͂�p����
					// ����ID�Ƃ��Ă� (kPrevOutputID | prevOutputIndex) ���w�肷����̂Ƃ���
					auto id = ids[i];
					if (id.isPrevOutput)
					{
						assert(prev_prod != nullptr);
						assert(id.index < prev_prod->GetOutputCount());

						id = ResourceID::CreatePrevOutputID(passNo - 1, id.index);
						prod->SetInput(i, id);			// ID���Z�b�g������
					}

					lastAccess[id] = passNo;
				}

				// �ꎞ���\�[�X�Ɏ����I��ID�����蓖�Ă�
				cnt = prod->GetTempCount();
				for (ym::u16 i = 0; i < cnt; ++i)
				{
					auto id = ResourceID::CreateTemporalID((ym::u8)passNo, (ym::u8)i);
					prod->SetTempID(i, id);
					lastAccess[id] = passNo;
				}

				prev_prod = prod;
				passNo++;
			}
		}

		// ���g�p���\�[�X����g������̂�T��
		std::vector<RenderResource *>::iterator FindFromUnusedResource(std::vector<RenderResource *> &unusedRes, const RenderResourceDesc &desc)
		{
			for (auto it = unusedRes.begin(); it != unusedRes.end(); it++)
			{
				if ((*it)->IsSameDesc(desc))
				{
					return it;
				}
			}
			return unusedRes.end();
		}

		// �q�X�g���[�o�b�t�@�̋L�q�q����������
		bool FindDescForHistoryBuffer(RenderResourceDesc *pOut, const std::vector<ResourceProducerBase *> &producers, ResourceID id)
		{
			id.historyOffset = 0;
			for (auto &&prod : producers)
			{
				// �o�̓o�b�t�@����������
				auto cnt = prod->GetOutputCount();
				auto ids = prod->GetOutputIds();
				for (u32 i = 0; i < cnt; i++)
				{
					if (ids[i] == id)
					{
						*pOut = prod->GetOutputDescs()[i];
						return true;
					}
				}
			}
			return false;
		}

		// ���\�[�X����
		void MakeResourcesDetail(
			ym::Device &device,
			ym::u32 screenWidth, ym::u32 screenHeight,
			const LastAccessPassInfo &lastAccess,
			std::vector<ResourceProducerBase *> &producers,
			std::vector<RenderResource *> &outResources,
			std::map<ResourceID, RenderResource *> &outResourceMap)
		{
			const D3D12_RESOURCE_STATES kInitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			const D3D12_RESOURCE_STATES kInputState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			const D3D12_RESOURCE_STATES kRtvState = D3D12_RESOURCE_STATE_RENDER_TARGET;
			const D3D12_RESOURCE_STATES kDsvState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			ym::u16 passNo = 0;
			std::map<ResourceID, RenderResource *> usedRes;
			std::vector<RenderResource *> unusedRes;

			// �O��t���[���̃��\�[�X���g�p�����X�g�Ɩ��g�p���X�g�ɐU�蕪����
			outResourceMap.clear();
			for (auto &&res : outResources)
			{
				if (res->IsHistoryEnd())
				{
					unusedRes.push_back(res);
				}
				else
				{
					auto id = res->GetLastID();
					id.historyOffset++;
					res->IncrementHistory();
					usedRes[id] = res;
					outResourceMap[id] = res;
				}
			}

			for (auto &&prod : producers)
			{
				// ���̓��\�[�X�������E��������
				auto cnt = prod->GetInputCount();
				auto ids = prod->GetInputIds();
				for (ym::u32 i = 0; i < cnt; i++)
				{
					auto id = ids[i];
					auto findIt = usedRes.find(id);
					if (findIt != usedRes.end())
					{
						// �g�p�����X�g�Ɍ��������̂ŏ�Ԃ�ۑ�
						auto res = findIt->second;
						prod->SetInputPrevState(i, res->GetState());
						res->SetState(kInputState);
					}
					else
					{
						// �܂��`�悳��Ă��Ȃ��q�X�g���[�o�b�t�@�̏ꍇ�͎g�p�����X�g�Ɍ�����Ȃ�
						assert(id.historyOffset > 0);
						// ���̓��\�[�X�ɂ͋L�q�q���w�肳��Ă��Ȃ��̂ŁA�v���f���[�T�[����Ή�����L�q�q����������
						// NOTE: �q�X�g���[�o�b�t�@�ɕ`�悷��p�X�����݂��Ȃ��͂��͂Ȃ����A���݂��Ȃ��ꍇ��Assert����
						RenderResourceDesc desc;
						bool isFindHBDesc = FindDescForHistoryBuffer(&desc, producers, id);
						assert(isFindHBDesc);
						// �V�K���\�[�X���쐬����
						auto res = new RenderResource();
						res->Initialize(device, desc, screenWidth, screenHeight, kInitialState);
						res->SetHistoryMax(0);
						res->SetLastID(id);
						outResources.push_back(res);
						usedRes[id] = res;
						outResourceMap[id] = res;
						prod->SetOutputPrevState(i, kInitialState);
					}
				}

				// �o�̓��\�[�X�������E��������
				cnt = prod->GetOutputCount();
				ids = prod->GetOutputIds();
				auto descs = prod->GetOutputDescs();
				for (ym::u32 i = 0; i < cnt; i++)
				{
					auto id = ids[i];
					auto findIt = usedRes.find(id);
					if (findIt != usedRes.end())
					{
						// ���\�[�X���g�p�����X�g���猩������
						auto res = findIt->second;
						if (!id.isSwapchain)
						{
							prod->SetOutputPrevState(i, res->GetState());
							res->SetState(res->IsRtv() ? kRtvState : kDsvState);
						}
						else
						{
							prod->SetOutputPrevState(i, kRtvState);
						}
					}
					else if (id.isSwapchain)
					{
						// �X���b�v�`�F�C�����g�p����p�X�����߂ēo�ꂵ���̂ŁA�O���Ԃ�Present�Ƃ���
						// NOTE: �X���b�v�`�F�C���̓q�X�g���[�𖳎�����
						prod->SetOutputPrevState(i, D3D12_RESOURCE_STATE_PRESENT);
						usedRes[id] = nullptr;
					}
					else
					{
						RenderResource *res;
						auto &&desc = descs[i];

						// ���g�p���\�[�X�Ɏg������̂����邩�`�F�b�N
						auto unusedIt = FindFromUnusedResource(unusedRes, desc);
						if (unusedIt != unusedRes.end())
						{
							// ���g�p���\�[�X�����������̂ŗ��p����
							res = *unusedIt;
							unusedRes.erase(unusedIt);
							prod->SetOutputPrevState(i, res->GetState());
						}
						else
						{
							// ���g�p���\�[�X�����݂��Ȃ��̂ŐV�K�쐬
							res = new RenderResource();
							res->Initialize(device, descs[i], screenWidth, screenHeight, kInitialState);
							outResources.push_back(res);
							prod->SetOutputPrevState(i, kInitialState);
						}

						// �g�p�����X�g�ɓo�^����
						res->SetState(res->IsRtv() ? kRtvState : kDsvState);
						res->SetHistoryMax(desc.historyMax);
						res->SetLastID(id);
						usedRes[id] = res;
						outResourceMap[id] = res;
					}
				}

				// �ꎞ���\�[�X�������E��������
				cnt = prod->GetTempCount();
				ids = prod->GetTempIds();
				descs = prod->GetTempDescs();
				for (ym::u32 i = 0; i < cnt; i++)
				{
					RenderResource *res;

					auto id = ids[i];
					auto unusedIt = FindFromUnusedResource(unusedRes, descs[i]);
					if (unusedIt != unusedRes.end())
					{
						// ���g�p���\�[�X�����������̂ŗ��p����
						res = *unusedIt;
						unusedRes.erase(unusedIt);
						prod->SetTempPrevState(i, res->GetState());
					}
					else
					{
						// ���g�p���\�[�X�����݂��Ȃ��̂ŐV�K�쐬
						res = new RenderResource();
						res->Initialize(device, descs[i], screenWidth, screenHeight, kInitialState);
						outResources.push_back(res);
						prod->SetTempPrevState(i, kInitialState);
					}

					// ���[�N���g�p�}�b�v�ɓo�^����
					res->SetState(kInputState);
					res->SetHistoryMax(0);
					res->SetLastID(id);
					usedRes[id] = res;
					outResourceMap[id] = res;
				}

				// �g�p�����X�g�𐮗�����
				auto usedIt = usedRes.begin();
				while (usedIt != usedRes.end())
				{
					if (usedIt->first.isSwapchain)
					{
						usedIt++;
						continue;
					}

					auto findIt = lastAccess.find(usedIt->first);
					if (usedIt->second->IsHistoryEnd() && (findIt != lastAccess.end()) && (findIt->second <= passNo))
					{
						// ���łɕs�v�ɂȂ������\�[�X�𖢎g�p���X�g�Ɉړ�����
						// NOTE: �q�X�g���[�o�b�t�@�͕ێ�����t���[�������o�߂���܂Ŗ��g�p�ɂł��Ȃ�
						unusedRes.push_back(usedIt->second);
						auto thisIt = usedIt;
						usedIt++;
						usedRes.erase(thisIt);
					}
					else
					{
						usedIt++;
					}
				}

				passNo++;
			}
		}

	}

	//-------------------------------------------
	// �ˑ��֌W����K�v�ȃ��\�[�X�𐶐�����
	//-------------------------------------------
	void RenderResourceManager::MakeResources(std::vector<ResourceProducerBase *> &producers)
	{
		assert(pDevice_ != nullptr);

		// �eID�̍ŏI�A�N�Z�X�p�X�ԍ����L�^����
		LastAccessPassInfo lastAccess;
		MakeResourceHistory(producers, lastAccess);

		// ���ۂ̃��\�[�X����
		MakeResourcesDetail(*pDevice_, screenWidth_, screenHeight_, lastAccess, producers, resources_, resource_map_);
	}

	//-------------------------------------------
	// ���̓��\�[�X�Ƀo���A�𒣂�
	//-------------------------------------------
	void RenderResourceManager::BarrierInputResources(CommandList &cmdList, const ResourceProducerBase *pProd)
	{
		auto count = pProd->GetInputCount();
		auto ids = pProd->GetInputIds();
		auto end = ids + count;
		auto prevState = pProd->GetInputPrevStates();
		for (; ids != end; ids++)
		{
			if (*prevState != (D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE))
			{
				auto res = GetRenderResourceFromID(*ids);
				cmdList.TransitionBarrier(res->GetTexture(), *prevState, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}
			prevState++;
		}
	}

	//-------------------------------------------
	// �o�̓��\�[�X�Ƀo���A�𒣂�
	//-------------------------------------------
	void RenderResourceManager::BarrierOutputResources(CommandList &cmdList, const ResourceProducerBase *pProd)
	{
		auto count = pProd->GetOutputCount();
		auto ids = pProd->GetOutputIds();
		auto end = ids + count;
		auto prevState = pProd->GetOutputPrevStates();
		for (; ids != end; ids++)
		{
			if (ids->isSwapchain)
			{
				if (*prevState == D3D12_RESOURCE_STATE_PRESENT)
				{
					auto scTex = pDevice_->GetSwapchain().GetCurrentTexture(1);
					cmdList.TransitionBarrier(scTex, *prevState, D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
			}
			else
			{
				auto res = GetRenderResourceFromID(*ids);
				if (res->IsRtv() && (*prevState != D3D12_RESOURCE_STATE_RENDER_TARGET))
				{
					cmdList.TransitionBarrier(res->GetTexture(), *prevState, D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
				else if (res->IsDsv() && (*prevState != D3D12_RESOURCE_STATE_DEPTH_WRITE))
				{
					cmdList.TransitionBarrier(res->GetTexture(), *prevState, D3D12_RESOURCE_STATE_DEPTH_WRITE);
				}
			}
			prevState++;
		}
	}

}	// namespace ym


//	EOF
