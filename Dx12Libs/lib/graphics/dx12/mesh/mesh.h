#pragma once

#include "format/meshFormat.h"
#include "../buffer/buffer.h"
#include "..\bufferView\bufferView.h"


namespace ym
{
	/***************************************//**
	 * @brief �V�F�C�v�C���X�^���X
	*******************************************/
	class MeshShapeInstance
	{
	private:
		struct VertexBuffer
		{
			Buffer				buffer_;
			VertexBufferView	view_;

			~VertexBuffer()
			{
				view_.Destroy();
				buffer_.Destroy();
			}
		};	// struct VertexBuffer

	public:
		MeshShapeInstance()
		{}
		~MeshShapeInstance()
		{
			Destroy();
		}

		/**
		 * @brief ����������
		*/
		bool Initialize(ym::Device *pDev, ym::CommandList *pCmdList, const MeshShape *shape, const void *p_vertex_head);

		/**
		 * @brief �j������
		*/
		void Destroy();

		//! @name �擾�֐�
		//! @{
		const MeshShape *GetSrcShape() const
		{
			return pSrcShape_;
		}
		VertexBufferView *GetPositionView()
		{
			return &vbPosition_.view_;
		}
		VertexBufferView *GetNormalView()
		{
			return &vbNormal_.view_;
		}
		VertexBufferView *GetTexcoordView()
		{
			return &vbTexcoord_.view_;
		}
		//! @}

	private:
		const MeshShape *pSrcShape_ = nullptr;

		VertexBuffer		vbPosition_;
		VertexBuffer		vbNormal_;
		VertexBuffer		vbTexcoord_;
	};	// class MeshShapeInstance

	/***************************************//**
	 * @brief �T�u���b�V���C���X�^���X
	*******************************************/
	class MeshSubmeshInstance
	{
	private:
		struct IndexBuffer
		{
			Buffer				buffer_;
			IndexBufferView		view_;

			~IndexBuffer()
			{
				view_.Destroy();
				buffer_.Destroy();
			}
		};	// struct VertexBuffer

	public:
		MeshSubmeshInstance()
		{}
		~MeshSubmeshInstance()
		{
			Destroy();
		}

		/**
		 * @brief ����������
		*/
		bool Initialize(ym::Device *pDev, ym::CommandList *pCmdList, const MeshSubmesh *shape, const void *p_vertex_head);

		/**
		 * @brief �j������
		*/
		void Destroy();

		//! @name �擾�֐�
		//! @{
		const MeshSubmesh *GetSrcSubmesh() const
		{
			return pSrcSubmesh_;
		}
		IndexBufferView *GetIndexBufferView()
		{
			return &indexBuffer_.view_;
		}
		//! @}

	private:
		const MeshSubmesh *pSrcSubmesh_ = nullptr;

		IndexBuffer			indexBuffer_;
	};	// class MeshSubmeshInstance

	/***************************************//**
	 * @brief �`��T�u���b�V�����
	*******************************************/
	struct DrawSubmeshInfo
	{
		MeshShapeInstance *pShape = nullptr;
		MeshSubmeshInstance *pSubmesh = nullptr;
		const MeshMaterial *pMaterial = nullptr;
		s32						numIndices = 0;
	};	// struct DrawSubmeshInfo

	/***************************************//**
	 * @brief ���b�V���C���X�^���X
	*******************************************/
	class MeshInstance
	{
	public:
		MeshInstance()
		{}
		~MeshInstance()
		{
			Destroy();
		}

		/**
		 * @brief ����������
		*/
		bool Initialize(ym::Device *pDev, ym::CommandList *pCmdList, const void *pBin);

		/**
		 * @brief �j������
		*/
		void Destroy();

		//! @name �擾�֐�
		//! @{
		const MeshHead *GetHead() const
		{
			assert(pHead_ != nullptr);
			return pHead_;
		}
		const MeshMaterial *GetMaterials() const
		{
			assert(pMaterials_ != nullptr);
			return pMaterials_;
		}
		const MeshShapeInstance *GetShapes() const
		{
			assert(pShapes_ != nullptr);
			return pShapes_;
		}
		const MeshSubmeshInstance *GetSubmeshes() const
		{
			assert(pSubmeshes_ != nullptr);
			return pSubmeshes_;
		}
		s32 GetSubmeshCount() const
		{
			assert(pHead_ != nullptr);
			return pHead_->numSubmeshes;
		}
		DrawSubmeshInfo GetDrawSubmeshInfo(s32 index) const
		{
			assert(pHead_ != nullptr);
			assert(pMaterials_ != nullptr);
			assert(pShapes_ != nullptr);
			assert(pSubmeshes_ != nullptr);
			assert(0 <= index && index < pHead_->numSubmeshes);

			DrawSubmeshInfo ret;
			ret.pSubmesh = pSubmeshes_ + index;
			ret.pShape = pShapes_ + ret.pSubmesh->GetSrcSubmesh()->shapeIndex;
			ret.pMaterial = pMaterials_ + ret.pSubmesh->GetSrcSubmesh()->materialIndex;
			ret.numIndices = ret.pSubmesh->GetSrcSubmesh()->numSubmeshIndices;

			return ret;
		}
		//! @}

	private:
		const MeshHead *pHead_ = nullptr;
		const MeshMaterial *pMaterials_ = nullptr;
		MeshShapeInstance *pShapes_ = nullptr;
		MeshSubmeshInstance *pSubmeshes_ = nullptr;
	};	// class MeshInstance

}	// namespace ym


//	EOF
