//#include "gui.h"
//#include "../graphics/dx12/device/device.h"
//#include "../graphics/dx12/commandList/commandList.h"
//#include "../graphics/dx12/texture/texture.h"
//#include "../graphics/dx12/textureView/textureView.h"
//#include "../graphics/dx12/buffer/buffer.h"
//#include "../graphics/dx12/bufferView/bufferView.h"
//#include "../graphics/dx12/sampler/sampler.h"
//#include "../graphics/dx12/shader/shader.h"
//#include "../graphics/dx12/descriptor/descriptor.h"
//#include "../graphics/dx12/descriptorHeap/descriptorHeap.h"
//#include "../graphics/dx12/descriptorSet/descriptorSet.h"
//#include "../graphics/dx12/swapChain/swapChain.h"
//#include "../graphics/dx12/rootSignature/rootSignature.h"
//#include "../graphics/dx12/pipelineState/pipelineState.h"
//#include "VSGui.h"
//#include "PSGui.h"
//
//
//namespace ym
//{
//	namespace
//	{
//		static const u32	kMaxFrameCount = Swapchain::kMaxBuffer;
//
//		struct VertexUniform
//		{
//			float	scale_[2];
//			float	translate_[2];
//		};	// struct VertexUniform
//
//		struct ImDrawVertex
//		{
//			float	pos_[2];
//			float	uv_[2];
//			u32		color_;
//		};	// struct ImDrawVertex
//
//	}	// namespace
//
//
//	Gui *Gui::guiHandle_ = nullptr;
//
//	//----
//	// ������
//	bool Gui::Initialize(Device *pDevice, DXGI_FORMAT rtFormat, DXGI_FORMAT dsFormat)
//	{
//		Destroy();
//
//		if (guiHandle_)
//		{
//			return false;
//		}
//
//		pOwner_ = pDevice;
//		guiHandle_ = this;
//
//		// imgui�̃R���e�L�X�g�쐬
//		ImGui::CreateContext();
//
//		// �R�[���o�b�N�̓o�^
//		ImGuiIO &io = ImGui::GetIO();
//		io.RenderDrawListsFn = &Gui::RenderDrawList;
//
//		// �V�F�[�_�쐬
//		pVShader_ = new Shader();
//		pPShader_ = new Shader();
//		if (!pVShader_ || !pPShader_)
//		{
//			return false;
//		}
//		if (!pVShader_->Initialize(pDevice, ShaderType::Vertex, kVSGui, sizeof(kVSGui)))
//		{
//			return false;
//		}
//		if (!pPShader_->Initialize(pDevice, ShaderType::Pixel, kPSGui, sizeof(kPSGui)))
//		{
//			return false;
//		}
//
//		// �T���v���쐬
//		{
//			pFontSampler_ = new Sampler();
//			if (!pFontSampler_)
//			{
//				return false;
//			}
//			D3D12_SAMPLER_DESC desc{};
//			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//			if (!pFontSampler_->Initialize(pDevice, desc))
//			{
//				return false;
//			}
//		}
//
//		// ���[�g�V�O�j�`���쐬
//		{
//			pRootSig_ = new RootSignature();
//			if (!pRootSig_->Initialize(pDevice, pVShader_, pPShader_, nullptr, nullptr, nullptr))
//			{
//				return false;
//			}
//
//			pDescSet_ = new DescriptorSet();
//		}
//
//		// �p�C�v���C���X�e�[�g�쐬
//		{
//			D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
//				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, 0,                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, sizeof(float) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//				{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 4, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//			};
//
//			GraphicsPipelineStateDesc desc{};
//
//			desc.blend.sampleMask = UINT_MAX;
//			desc.blend.rtDesc[0].isBlendEnable = true;
//			desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_SRC_ALPHA;
//			desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_INV_SRC_ALPHA;
//			desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
//			desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ONE;
//			desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
//			desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
//			desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//			desc.depthStencil.isDepthEnable = (dsFormat != DXGI_FORMAT_UNKNOWN);
//			desc.depthStencil.isDepthWriteEnable = (dsFormat != DXGI_FORMAT_UNKNOWN);
//			desc.depthStencil.depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//
//			desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
//			desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
//			desc.rasterizer.isFrontCCW = false;
//			desc.rasterizer.isDepthClipEnable = true;
//			desc.multisampleCount = 1;
//
//			desc.inputLayout.numElements = ARRAYSIZE(elementDescs);
//			desc.inputLayout.pElements = elementDescs;
//
//			desc.pRootSignature = pRootSig_;
//			desc.pVS = pVShader_;
//			desc.pPS = pPShader_;
//			desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//			desc.numRTVs = 0;
//			desc.rtvFormats[desc.numRTVs++] = rtFormat;
//			desc.dsvFormat = dsFormat;
//
//			pPipelineState_ = new GraphicsPipelineState();
//			if (!pPipelineState_->Initialize(pDevice, desc))
//			{
//				return false;
//			}
//		}
//
//		// �萔�o�b�t�@���쐬
//		pConstantBuffers_ = new Buffer[kMaxFrameCount];
//		pConstantBufferViews_ = new ConstantBufferView[kMaxFrameCount];
//		if (!pConstantBuffers_ || !pConstantBufferViews_)
//		{
//			return false;
//		}
//		for (u32 i = 0; i < kMaxFrameCount; i++)
//		{
//			if (!pConstantBuffers_[i].Initialize(pDevice, sizeof(VertexUniform), 0, BufferUsage::ConstantBuffer, true, false))
//			{
//				return false;
//			}
//			if (!pConstantBufferViews_[i].Initialize(pDevice, &pConstantBuffers_[i]))
//			{
//				return false;
//			}
//		}
//
//		// ���_�E�C���f�b�N�X�o�b�t�@���쐬
//		pVertexBuffers_ = new Buffer[kMaxFrameCount];
//		pVertexBufferViews_ = new VertexBufferView[kMaxFrameCount];
//		pIndexBuffers_ = new Buffer[kMaxFrameCount];
//		pIndexBufferViews_ = new IndexBufferView[kMaxFrameCount];
//		if (!pVertexBuffers_ || !pVertexBufferViews_ || !pIndexBuffers_ || !pIndexBufferViews_)
//		{
//			return false;
//		}
//
//		return true;
//	}
//
//	//----
//	// �j��
//	void Gui::Destroy()
//	{
//		if (pOwner_)
//		{
//			ym::SafeDelete(pVShader_);
//			ym::SafeDelete(pPShader_);
//			ym::SafeDelete(pFontTextureView_);
//			ym::SafeDelete(pFontTexture_);
//			ym::SafeDelete(pFontSampler_);
//
//			ym::SafeDeleteArray(pConstantBufferViews_);
//			ym::SafeDeleteArray(pConstantBuffers_);
//			ym::SafeDeleteArray(pVertexBufferViews_);
//			ym::SafeDeleteArray(pVertexBuffers_);
//			ym::SafeDeleteArray(pIndexBufferViews_);
//			ym::SafeDeleteArray(pIndexBuffers_);
//
//			ym::SafeDelete(pRootSig_);
//			ym::SafeDelete(pPipelineState_);
//			ym::SafeDelete(pDescSet_);
//
//			ImGui::DestroyContext();
//
//			pOwner_ = nullptr;
//		}
//		guiHandle_ = nullptr;
//	}
//
//	//----
//	// �t�H���g�C���[�W����
//	bool Gui::CreateFontImage(Device *pDevice, CommandList &cmdList)
//	{
//		if (!pOwner_)
//		{
//			return false;
//		}
//
//		ImGuiIO &io = ImGui::GetIO();
//
//		unsigned char *pixels;
//		int width, height;
//		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
//		size_t upload_size = width * height * 4 * sizeof(char);
//
//		// �e�N�X�`���쐬
//		TextureDesc desc;
//		desc.dimension = TextureDimension::Texture2D;
//		desc.width = static_cast<u32>(width);
//		desc.height = static_cast<u32>(height);
//		desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
//
//		pFontTexture_ = new Texture();
//		if (!pFontTexture_)
//		{
//			return false;
//		}
//
//		if (!pFontTexture_->InitializeFromImageBin(pDevice, &cmdList, desc, pixels))
//		{
//			return false;
//		}
//
//		// �e�N�X�`���r���[�쐬
//		pFontTextureView_ = new TextureView();
//		if (!pFontTextureView_)
//		{
//			return false;
//		}
//		if (!pFontTextureView_->Initialize(pDevice, pFontTexture_))
//		{
//			return false;
//		}
//
//		// �o�^
//		io.Fonts->SetTexID(pFontTexture_);
//
//		return true;
//	}
//
//
//	//----
//	void Gui::RenderDrawList(ImDrawData *draw_data)
//	{
//		ImGuiIO &io = ImGui::GetIO();
//
//		Gui *pThis = guiHandle_;
//		Device *pDevice = pThis->pOwner_;
//		CommandList *pCmdList = pThis->pDrawCommandList_;
//		u32 frameIndex = pThis->frameIndex_;
//
//		// ���_�o�b�t�@����
//		Buffer &vbuffer = pThis->pVertexBuffers_[frameIndex];
//		VertexBufferView &vbView = pThis->pVertexBufferViews_[frameIndex];
//		size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
//		if (vbuffer.GetSize() < vertex_size)
//		{
//			vbuffer.Destroy();
//			vbuffer.Initialize(pDevice, vertex_size, sizeof(ImDrawVert), BufferUsage::VertexBuffer, true, false);
//
//			vbView.Destroy();
//			vbView.Initialize(pDevice, &vbuffer);
//		}
//
//		// �C���f�b�N�X�o�b�t�@����
//		Buffer &ibuffer = pThis->pIndexBuffers_[frameIndex];
//		IndexBufferView &ibView = pThis->pIndexBufferViews_[frameIndex];
//		size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
//		if (ibuffer.GetSize() < index_size)
//		{
//			ibuffer.Destroy();
//			ibuffer.Initialize(pDevice, index_size, sizeof(ImDrawIdx), BufferUsage::IndexBuffer, true, false);
//
//			ibView.Destroy();
//			ibView.Initialize(pDevice, &ibuffer);
//		}
//
//		// ���_�E�C���f�b�N�X�̃��������㏑��
//		{
//			ImDrawVert *vtx_dst = static_cast<ImDrawVert *>(vbuffer.Map(pCmdList));
//			ImDrawIdx *idx_dst = static_cast<ImDrawIdx *>(ibuffer.Map(pCmdList));
//
//			for (int n = 0; n < draw_data->CmdListsCount; n++)
//			{
//				const ImDrawList *cmd_list = draw_data->CmdLists[n];
//				memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
//				memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
//				vtx_dst += cmd_list->VtxBuffer.Size;
//				idx_dst += cmd_list->IdxBuffer.Size;
//			}
//
//			vbuffer.Unmap();
//			ibuffer.Unmap();
//		}
//
//		// �萔�o�b�t�@�X�V
//		{
//			Buffer &cb = pThis->pConstantBuffers_[frameIndex];
//			float *p = static_cast<float *>(cb.Map(pCmdList));
//			p[0] = 2.0f / io.DisplaySize.x;
//			p[1] = -2.0f / io.DisplaySize.y;
//			p[2] = -1.0f;
//			p[3] = 1.0f;
//			cb.Unmap();
//		}
//
//		// �����_�����O�J�n
//		// NOTE: �����_�[�^�[�Q�b�g�͐ݒ�ς݂Ƃ���
//
//		// �p�C�v���C���X�e�[�g�ݒ�
//		ID3D12GraphicsCommandList *pNativeCmdList = pCmdList->GetCommandList();
//		pNativeCmdList->SetPipelineState(pThis->pPipelineState_->GetPSO());
//
//		// DescriptorSet�ݒ�
//		ConstantBufferView &cbView = pThis->pConstantBufferViews_[frameIndex];
//		pThis->pDescSet_->Reset();
//		pThis->pDescSet_->SetVsCbv(0, cbView.GetDescInfo().cpuHandle);
//		pThis->pDescSet_->SetPsSrv(0, pThis->pFontTextureView_->GetDescInfo().cpuHandle);
//		pThis->pDescSet_->SetPsSampler(0, pThis->pFontSampler_->GetDescInfo().cpuHandle);
//
//		// RootSig��DescSet��ݒ�
//		pCmdList->SetGraphicsRootSignatureAndDescriptorSet(pThis->pRootSig_, pThis->pDescSet_);
//
//		// DrawCall
//		D3D12_VERTEX_BUFFER_VIEW views[] = { vbView.GetView() };
//		pNativeCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		pNativeCmdList->IASetVertexBuffers(0, _countof(views), views);
//		pNativeCmdList->IASetIndexBuffer(&ibView.GetView());
//
//		// DrawCall
//		int vtx_offset = 0;
//		int idx_offset = 0;
//		for (int n = 0; n < draw_data->CmdListsCount; n++)
//		{
//			const ImDrawList *cmd_list = draw_data->CmdLists[n];
//			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
//			{
//				const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
//				if (pcmd->UserCallback)
//				{
//					pcmd->UserCallback(cmd_list, pcmd);
//				}
//				else
//				{
//					D3D12_RECT rect;
//					rect.left = static_cast<s32>(pcmd->ClipRect.x);
//					rect.top = static_cast<s32>(pcmd->ClipRect.y);
//					rect.right = static_cast<s32>(pcmd->ClipRect.z);
//					rect.bottom = static_cast<s32>(pcmd->ClipRect.w);
//					pNativeCmdList->RSSetScissorRects(1, &rect);
//
//					pNativeCmdList->DrawIndexedInstanced(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
//				}
//				idx_offset += pcmd->ElemCount;
//			}
//			vtx_offset += cmd_list->VtxBuffer.Size;
//		}
//	}
//
//	//----
//	// �V�����t���[���̊J�n
//	void Gui::BeginNewFrame(CommandList *pDrawCmdList, uint32_t frameWidth, uint32_t frameHeight, const InputData &input, float frameScale, float timeStep)
//	{
//		ImGuiIO &io = ImGui::GetIO();
//
//		// �t���[���o�b�t�@�̃T�C�Y���w�肷��
//		io.DisplaySize = ImVec2((float)frameWidth, (float)frameHeight);
//		io.DisplayFramebufferScale = ImVec2(frameScale, frameScale);
//
//		// ���Ԑi�s���w��
//		io.DeltaTime = timeStep;
//
//		// �}�E�X�ɂ�鑀��
//		io.MousePos = ImVec2((float)input.mouseX, (float)input.mouseY);
//		io.MouseDown[0] = (input.mouseButton & MouseButton::Left) != 0;
//		io.MouseDown[1] = (input.mouseButton & MouseButton::Right) != 0;
//		io.MouseDown[2] = (input.mouseButton & MouseButton::Middle) != 0;
//
//		// �V�K�t���[���J�n
//		ImGui::NewFrame();
//
//		frameIndex_ = (frameIndex_ + 1) % kMaxFrameCount;
//		pDrawCommandList_ = pDrawCmdList;
//	}
//
//}	// namespace ym
//
//
//// eof
