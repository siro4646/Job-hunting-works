#pragma once

#include "../device/device.h"
#include "../texture/texture.h"
#include "../textureView/textureView.h"
#include <vector>
#include <map>


namespace ym
{
	/************************************************//**
	 * @brief ���\�[�XID
	 *
	 * �`�惊�\�[�X�̎��ʎq.
	 * ���j�[�N���񃆃j�[�N���A�ꎞ���\�[�X���Ȃǂɂ���Ďg�p����ϐ��ɈႢ������.
	****************************************************/
	union ResourceID
	{
		ym::u32				id;						//!< ���\�[�X�S�̂�ID
		struct
		{
			union
			{
				ym::u16		uniqueID;				//!< ���\�[�X�ɖ�������ۂ̃��j�[�NID
				struct
				{
					ym::u8	passNo;					//!< �O�p�X�o�́A�������͈ꎞ���\�[�X�̎�͌��p�X�ԍ�
					ym::u8	index;					//!< �O�p�X�o�́A�������͈ꎞ���\�[�X�̃��\�[�X�C���f�b�N�X
				};
			};
			ym::u8			historyOffset;			//!< �q�X�g���[�o�b�t�@�̃I�t�Z�b�g(0�Ȃ猻�݃t���[���̃o�b�t�@�A1�`�̒l�͉��t���[���k�����o�b�t�@���g�p���邩)
			ym::u8			isPrevOutput : 1;	//!< �O�p�X�o�͂��g�p����t���O
			ym::u8			isTemporal : 1;	//!< �ꎞ���\�[�X�t���O
			ym::u8			isSwapchain : 1;	//!< �X���b�v�`�F�C���t���O
		};

		// operators
		bool operator==(const ResourceID &x) const
		{
			return id == x.id;
		}
		bool operator!=(const ResourceID &x) const
		{
			return id != x.id;
		}
		bool operator<(const ResourceID &x) const
		{
			return id < x.id;
		}

		// �e�탊�\�[�XID��������
		static ResourceID CreateUniqueID(ym::u16 unique_id)
		{
			ResourceID ret{};
			ret.uniqueID = unique_id;
			return ret;
		}
		static ResourceID CreateUniqueID(ym::u16 unique_id, ym::u8 history_offset)
		{
			ResourceID ret{};
			ret.uniqueID = unique_id;
			ret.historyOffset = history_offset;
			return ret;
		}
		static ResourceID CreatePrevOutputID(ym::u8 pass_no, ym::u8 prev_index)
		{
			ResourceID ret{};
			ret.passNo = pass_no;
			ret.index = prev_index;
			ret.isPrevOutput = 1;
			return ret;
		}
		static ResourceID CreateTemporalID(ym::u8 pass_no, ym::u8 temp_index)
		{
			ResourceID ret{};
			ret.passNo = pass_no;
			ret.index = temp_index;
			ret.isTemporal = 1;
			return ret;
		}
		static ResourceID CreateSwapchainID()
		{
			ResourceID ret{};
			ret.isSwapchain = 1;
			return ret;
		}
	};	// struct ResourceID


	/************************************************//**
	 * @brief �`�惊�\�[�X�L�q�q
	 *
	 * �`�惊�\�[�X�����A�������Ɏg�p����.
	****************************************************/
	struct RenderResourceDesc
	{
		ym::u32		width, height;				//!< �o�b�t�@�̕��ƍ���
		float			resolution_rate;			//!< �X�N���[���ɑ΂���𑜓x�̊���(0�ȉ��̏ꍇ�� width, height ���g�p����)
		ym::u32		mipLevels;					//!< �~�b�v���x��(> 0)
		DXGI_FORMAT		format;						//!< �t�H�[�}�b�g
		ym::u32		sampleCount;				//!< �T���v����
		ym::u32		targetCount;				//!< RTV�A��������DSV�̐�(�~�b�v���x���ȉ�)
		ym::u32		srvCount;					//!< SRV�̐�(�~�b�v���x��+1�ȉ�)
		ym::u32		uavCount;					//!< UAV�̐�(�~�b�v���x���ȉ�)
		ym::u32		historyMax;					//!< �q�X�g���[�o�b�t�@�Ƃ��Ă̕ێ��t���[����(0�Ȃ猻�݃t���[���̂ݎg�p����)

		RenderResourceDesc()
			: width(0), height(0)
			, resolution_rate(1.0f)
			, mipLevels(1)
			, format(DXGI_FORMAT_UNKNOWN)
			, sampleCount(1)
			, targetCount(1)
			, srvCount(1)
			, uavCount(0)
			, historyMax(0)
		{}

		bool operator==(const RenderResourceDesc &d) const
		{
			if (resolution_rate > 0.0f)
			{
				if (resolution_rate != d.resolution_rate) { return false; }
			}
			else
			{
				if (width != d.width || height != d.height) { return false; }
			}
			// NOTE: �q�X�g���[������v���Ȃ��Ă�����L�q�q�Ƃ݂Ȃ�
			return (mipLevels == d.mipLevels)
				&& (format == d.format)
				&& (sampleCount == d.sampleCount)
				&& (targetCount == d.targetCount)
				&& (srvCount == d.srvCount)
				&& (uavCount == d.uavCount);
		}

		RenderResourceDesc &SetSize(ym::u32 w, ym::u32 h)
		{
			width = w;
			height = h;
			return *this;
		}
		RenderResourceDesc &SetResolutionRate(float r)
		{
			resolution_rate = r;
			return *this;
		}
		RenderResourceDesc &SetMipLevels(ym::u32 m)
		{
			mipLevels = m;
			return *this;
		}
		RenderResourceDesc &SetFormat(DXGI_FORMAT f)
		{
			format = f;
			return *this;
		}
		RenderResourceDesc &SetSampleCount(ym::u32 c)
		{
			sampleCount = c;
			return *this;
		}
		RenderResourceDesc &SetTargetCount(ym::u32 c)
		{
			targetCount = c;
			return *this;
		}
		RenderResourceDesc &SetSrvCount(ym::u32 c)
		{
			srvCount = c;
			return *this;
		}
		RenderResourceDesc &SetUavCount(ym::u32 c)
		{
			uavCount = c;
			return *this;
		}
		RenderResourceDesc &SetHistoryMax(ym::u32 c)
		{
			historyMax = c;
			return *this;
		}
	};	// struct RenderResourceDesc

	/************************************************//**
	 * @brief �`�惊�\�[�X
	 *
	 * RTV�A��������DSV.\n
	 * �e�N�X�`���I�u�W�F�N�g��RTV or DSV, SRV(, UAV)��ێ�����.
	****************************************************/
	class RenderResource
	{
	public:
		RenderResource()
		{}
		~RenderResource()
		{
			Destroy();
		}

		bool Initialize(ym::Device &device, const RenderResourceDesc &desc, ym::u32 screenWidth, ym::u32 screenHeight, D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		void Destroy();

		bool IsSameDesc(const RenderResourceDesc &d) const
		{
			return d == desc_;
		}

		bool IsRtv() const
		{
			return !rtvs_.empty();
		}

		bool IsDsv() const
		{
			return !dsvs_.empty();
		}

		bool IsUav() const
		{
			return !uavs_.empty();
		}

		//! @name �ݒ�֐�
		//! @{
		void SetState(D3D12_RESOURCE_STATES s)
		{
			state_ = s;
		}
		void SetLastID(ResourceID id)
		{
			lastID_ = id;
		}
		void SetHistoryMax(int n)
		{
			history_ = 0;
			historyMax_ = n;
		}
		void IncrementHistory()
		{
			history_++;
		}
		//! @}

		//! @name �擾�֐�
		//! @{
		ym::Texture *GetTexture()
		{
			return &texture_;
		}
		ym::RenderTargetView *GetRtv(int index = 0)
		{
			return &rtvs_[index];
		}
		ym::DepthStencilView *GetDsv(int index = 0)
		{
			return &dsvs_[index];
		}
		ym::TextureView *GetSrv(int index = 0)
		{
			return &srvs_[index];
		}
		ym::UnorderedAccessView *GetUav(int index = 0)
		{
			return &uavs_[index];
		}
		D3D12_RESOURCE_STATES GetState() const
		{
			return state_;
		}
		ResourceID GetLastID() const
		{
			return lastID_;
		}
		bool IsHistoryEnd() const
		{
			return history_ >= historyMax_;
		}
		//! @}

	private:
		RenderResourceDesc						desc_;

		ym::Texture							texture_;
		std::vector<ym::RenderTargetView>		rtvs_;
		std::vector<ym::DepthStencilView>		dsvs_;
		std::vector<ym::TextureView>			srvs_;
		std::vector<ym::UnorderedAccessView>	uavs_;

		D3D12_RESOURCE_STATES					state_;				//!< ���̏�Ԃ̓��\�[�X�������ɎQ�Ƃ����
		ResourceID								lastID_;			//!< �Ō�Ɏg�p���ꂽ����ID
		int										history_ = 0;		//!< ���݂̃q�X�g���[�ԍ�(�i�s�t���[��)
		int										historyMax_ = 0;	//!< �q�X�g���[�Ƃ��ĕۑ�����ő�t���[��
	};	// class RenderResource

	/************************************************//**
	 * @brief ���\�[�X�v���f���[�T�[���
	 *
	 * ���\�[�X��R/W���Ǘ�����v���f���[�T�[�N���X�̊��ł�.\n
	 * �C���^�[�t�F�[�X�Ƃ��ċ@�\���܂�.
	****************************************************/
	class ResourceProducerBase
	{
	public:
		//! @name �擾�֐�
		//! @{
		ym::u32 GetInputCount() const
		{
			return inputCount_;
		}
		const ResourceID *GetInputIds() const
		{
			return pInputIds_;
		}
		const D3D12_RESOURCE_STATES *GetInputPrevStates() const
		{
			return pInputPrevStates_;
		}

		ym::u32 GetOutputCount() const
		{
			return outputCount_;
		}
		const ResourceID *GetOutputIds() const
		{
			return pOutputIds_;
		}
		const RenderResourceDesc *GetOutputDescs() const
		{
			return pOutputDescs_;
		}
		const D3D12_RESOURCE_STATES *GetOutputPrevStates() const
		{
			return pOutputPrevStates_;
		}

		ym::u32 GetTempCount() const
		{
			return tempCount_;
		}
		const ResourceID *GetTempIds() const
		{
			return pTempIds_;
		}
		const RenderResourceDesc *GetTempDescs() const
		{
			return pTempDescs_;
		}
		const D3D12_RESOURCE_STATES *GetTempPrevStates() const
		{
			return pTempPrevStates_;
		}
		//! @}

		//! @name �ݒ�֐�
		//! @{
		void SetInput(ym::u32 index, ResourceID id)
		{
			assert(index < inputCount_);
			pInputIds_[index] = id;
		}
		void SetInputUnique(ym::u32 index, ym::u16 unique_id)
		{
			assert(index < inputCount_);
			pInputIds_[index] = ResourceID::CreateUniqueID(unique_id);
		}
		void SetInputUnique(ym::u32 index, ym::u16 unique_id, ym::u8 history)
		{
			assert(index < inputCount_);
			pInputIds_[index] = ResourceID::CreateUniqueID(unique_id, history);
		}
		void SetInputFromPrevOutput(ym::u32 index, ym::u32 prevOutputIndex)
		{
			assert(index < inputCount_);
			pInputIds_[index] = ResourceID::CreatePrevOutputID(0, prevOutputIndex);
		}
		void SetInputPrevState(ym::u32 index, D3D12_RESOURCE_STATES state)
		{
			assert(index < inputCount_);
			pInputPrevStates_[index] = state;
		}

		void SetOutput(ym::u32 index, ResourceID id, const RenderResourceDesc &desc)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = id;
			pOutputDescs_[index] = desc;
		}
		void SetOutputUnique(ym::u32 index, ym::u16 unique_id, const RenderResourceDesc &desc)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = ResourceID::CreateUniqueID(unique_id);
			pOutputDescs_[index] = desc;
		}
		void SetOutputUnique(ym::u32 index, ym::u16 unique_id, ym::u8 history, const RenderResourceDesc &desc)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = ResourceID::CreateUniqueID(unique_id, history);
			pOutputDescs_[index] = desc;
		}
		void SetOutputForNextPass(ym::u32 index, const RenderResourceDesc &desc)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = ResourceID::CreatePrevOutputID(0, (ym::u8)index);
			pOutputDescs_[index] = desc;
		}
		void SetOutputSwapchain(ym::u32 index)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = ResourceID::CreateSwapchainID();
		}
		void SetOutputID(ym::u32 index, ResourceID id)
		{
			assert(index < outputCount_);
			pOutputIds_[index] = id;
		}
		void SetOutputPrevState(ym::u32 index, D3D12_RESOURCE_STATES state)
		{
			assert(index < outputCount_);
			pOutputPrevStates_[index] = state;
		}

		void SetTemp(ym::u32 index, const RenderResourceDesc &desc)
		{
			assert(index < tempCount_);
			pTempDescs_[index] = desc;
		}
		void SetTempID(ym::u32 index, ResourceID id)
		{
			assert(index < tempCount_);
			pTempIds_[index] = id;
		}
		void SetTempPrevState(ym::u32 index, D3D12_RESOURCE_STATES state)
		{
			assert(index < tempCount_);
			pTempPrevStates_[index] = state;
		}
		//! @}

	protected:
		ResourceProducerBase(
			ym::u32 inputCount, ym::u32 outputCount, ym::u32 tempCount,
			ResourceID *resIds, RenderResourceDesc *resDescs, D3D12_RESOURCE_STATES *pPrevStates)
			: inputCount_(inputCount), pInputIds_(resIds), pInputPrevStates_(pPrevStates)
			, outputCount_(outputCount), pOutputIds_(resIds + inputCount), pOutputDescs_(resDescs), pOutputPrevStates_(pPrevStates + inputCount)
			, tempCount_(tempCount), pTempIds_(resIds + inputCount + outputCount), pTempDescs_(resDescs + outputCount), pTempPrevStates_(pPrevStates + inputCount + outputCount)
		{}

	protected:
		ym::u32				inputCount_;
		ResourceID *pInputIds_;
		D3D12_RESOURCE_STATES *pInputPrevStates_;

		ym::u32				outputCount_;
		ResourceID *pOutputIds_;
		RenderResourceDesc *pOutputDescs_;
		D3D12_RESOURCE_STATES *pOutputPrevStates_;

		ym::u32				tempCount_;
		ResourceID *pTempIds_;
		RenderResourceDesc *pTempDescs_;
		D3D12_RESOURCE_STATES *pTempPrevStates_;
	};	// class ResourceProducerBase

	/************************************************//**
	 * @brief ���\�[�X�v���f���[�T�[
	 *
	 * ���N���X�Ŏ�舵���f�[�^��ێ�����e���v���[�g�N���X.
	****************************************************/
	template <ym::u32 InputCount, ym::u32 OutputCount, ym::u32 TempCount>
	class ResourceProducer
		: public ResourceProducerBase
	{
	public:
		ResourceProducer()
			: ResourceProducerBase(InputCount, OutputCount, TempCount, resourceIds_, resourceDescs_, prevStates_)
		{}

	private:
		ResourceID				resourceIds_[InputCount + OutputCount + TempCount];
		RenderResourceDesc		resourceDescs_[OutputCount + TempCount];
		D3D12_RESOURCE_STATES	prevStates_[InputCount + OutputCount + TempCount];
	};	// class ResourceProducer

	/************************************************//**
	 * @brief �`�惊�\�[�X�}�l�[�W��
	 *
	 * �`�惊�\�[�X�̐����ƊǗ����s��.\n
	 * ��{�I�ɁA��x���������`�惊�\�[�X�͍폜���Ȃ�.
	****************************************************/
	class RenderResourceManager
	{
	public:
		RenderResourceManager()
		{}
		~RenderResourceManager()
		{
			Destroy();
		}

		bool Initialize(ym::Device &device, ym::u32 screenWidth, ym::u32 screenHeight);
		void Destroy();

		void MakeResources(std::vector<ResourceProducerBase *> &producers);

		/**
		 * @brief ���̓��\�[�X�Ƀo���A�𒣂�
		*/
		void BarrierInputResources(CommandList &cmdList, const ResourceProducerBase *pProd);

		/**
		 * @brief �o�̓��\�[�X�Ƀo���A�𒣂�
		*/
		void BarrierOutputResources(CommandList &cmdList, const ResourceProducerBase *pProd);

		/**
		 * @brief ���o�̓��\�[�X�Ƀo���A�𒣂�
		*/
		void BarrierAllResources(CommandList &cmdList, const ResourceProducerBase *pProd)
		{
			BarrierInputResources(cmdList, pProd);
			BarrierOutputResources(cmdList, pProd);
		}

		/**
		 * @brief ID����`�惊�\�[�X���擾����
		*/
		RenderResource *GetRenderResourceFromID(ResourceID id)
		{
			auto it = resource_map_.find(id);
			return (it != resource_map_.end()) ? it->second : nullptr;
		}

	private:
		void AllReset();

	private:
		ym::Device *pDevice_;
		ym::u32								screenWidth_, screenHeight_;

		std::vector<RenderResource *>			resources_;
		std::map<ResourceID, RenderResource *>	resource_map_;
	};	// class RenderResourceManager

}	// namespace ym


//	EOF
