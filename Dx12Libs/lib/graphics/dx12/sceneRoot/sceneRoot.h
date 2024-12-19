#pragma once

#include "../renderCommand/renderCommand.h"


namespace ym
{
	using RenderCommandPtr = std::unique_ptr<ym::RenderCommand>;
	using RenderCommandsList = std::vector<RenderCommandPtr>;
	using RenderCommandsTempList = std::vector<ym::RenderCommand *>;

	//----------------
	class SceneNode
	{
	public:
		SceneNode()
		{}
		virtual ~SceneNode()
		{}

		// need to call when new frame begin.
		virtual void BeginNewFrame(CommandList *pCmdList)
		{}

		// create unique render command.
		virtual void CreateRenderCommand(ConstantBufferCache *pCBCache, RenderCommandsList &outRenderCmds)
		{}
	};	// class SceneNode

	using SceneNodePtr = std::shared_ptr<SceneNode>;
	using SceneNodeWeakPtr = std::weak_ptr<SceneNode>;

	//----------------
	class SceneRoot
	{
	public:
		SceneRoot();
		~SceneRoot();

		void AttachNode(SceneNodePtr node);
		void DeleteNode(SceneNodePtr node);

		void BeginNewFrame(CommandList *pCmdList);

		void GatherRenderCommands(ConstantBufferCache *pCBCache, RenderCommandsList &outRenderCmds);

		void GabageCollect();

		bool IsDirty() const
		{
			return bDirty_;
		}

	private:
		std::vector<SceneNodeWeakPtr>	nodes_;
		bool							bDirty_;
		bool							bDirty_PrevFrame_;
	};	// class SceneRoot

}	// namespace ym


//	EOF
