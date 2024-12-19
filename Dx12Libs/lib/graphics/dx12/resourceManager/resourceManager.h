#pragma once

#include "../device/device.h"
#include "../texture/texture.h"
#include "../textureView/textureView.h"
#include <vector>
#include <map>


namespace ym
{
	/************************************************//**
	 * @brief リソースID
	 *
	 * 描画リソースの識別子.
	 * ユニークか非ユニークか、一時リソースかなどによって使用する変数に違いがある.
	****************************************************/
	union ResourceID
	{
		ym::u32				id;						//!< リソース全体のID
		struct
		{
			union
			{
				ym::u16		uniqueID;				//!< リソースに命名する際のユニークID
				struct
				{
					ym::u8	passNo;					//!< 前パス出力、もしくは一時リソースの主力元パス番号
					ym::u8	index;					//!< 前パス出力、もしくは一時リソースのリソースインデックス
				};
			};
			ym::u8			historyOffset;			//!< ヒストリーバッファのオフセット(0なら現在フレームのバッファ、1～の値は何フレーム遡ったバッファを使用するか)
			ym::u8			isPrevOutput : 1;	//!< 前パス出力を使用するフラグ
			ym::u8			isTemporal : 1;	//!< 一時リソースフラグ
			ym::u8			isSwapchain : 1;	//!< スワップチェインフラグ
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

		// 各種リソースID生成命令
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
	 * @brief 描画リソース記述子
	 *
	 * 描画リソース生成、検索時に使用する.
	****************************************************/
	struct RenderResourceDesc
	{
		ym::u32		width, height;				//!< バッファの幅と高さ
		float			resolution_rate;			//!< スクリーンに対する解像度の割合(0以下の場合は width, height を使用する)
		ym::u32		mipLevels;					//!< ミップレベル(> 0)
		DXGI_FORMAT		format;						//!< フォーマット
		ym::u32		sampleCount;				//!< サンプル数
		ym::u32		targetCount;				//!< RTV、もしくはDSVの数(ミップレベル以下)
		ym::u32		srvCount;					//!< SRVの数(ミップレベル+1以下)
		ym::u32		uavCount;					//!< UAVの数(ミップレベル以下)
		ym::u32		historyMax;					//!< ヒストリーバッファとしての保持フレーム数(0なら現在フレームのみ使用する)

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
			// NOTE: ヒストリー数が一致しなくても同一記述子とみなす
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
	 * @brief 描画リソース
	 *
	 * RTV、もしくはDSV.\n
	 * テクスチャオブジェクトとRTV or DSV, SRV(, UAV)を保持する.
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

		//! @name 設定関数
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

		//! @name 取得関数
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

		D3D12_RESOURCE_STATES					state_;				//!< この状態はリソース生成時に参照される
		ResourceID								lastID_;			//!< 最後に使用された時のID
		int										history_ = 0;		//!< 現在のヒストリー番号(進行フレーム)
		int										historyMax_ = 0;	//!< ヒストリーとして保存する最大フレーム
	};	// class RenderResource

	/************************************************//**
	 * @brief リソースプロデューサー基底
	 *
	 * リソースのR/Wを管理するプロデューサークラスの基底です.\n
	 * インターフェースとして機能します.
	****************************************************/
	class ResourceProducerBase
	{
	public:
		//! @name 取得関数
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

		//! @name 設定関数
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
	 * @brief リソースプロデューサー
	 *
	 * 基底クラスで取り扱うデータを保持するテンプレートクラス.
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
	 * @brief 描画リソースマネージャ
	 *
	 * 描画リソースの生成と管理を行う.\n
	 * 基本的に、一度生成した描画リソースは削除しない.
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
		 * @brief 入力リソースにバリアを張る
		*/
		void BarrierInputResources(CommandList &cmdList, const ResourceProducerBase *pProd);

		/**
		 * @brief 出力リソースにバリアを張る
		*/
		void BarrierOutputResources(CommandList &cmdList, const ResourceProducerBase *pProd);

		/**
		 * @brief 入出力リソースにバリアを張る
		*/
		void BarrierAllResources(CommandList &cmdList, const ResourceProducerBase *pProd)
		{
			BarrierInputResources(cmdList, pProd);
			BarrierOutputResources(cmdList, pProd);
		}

		/**
		 * @brief IDから描画リソースを取得する
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
