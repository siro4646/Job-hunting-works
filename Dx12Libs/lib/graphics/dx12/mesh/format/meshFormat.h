#pragma once


namespace ym
{
	/**********************************************//**
	 * @brief �V�F�C�v
	**************************************************/
	struct MeshShape
	{
		char	name[64];
		u32		numVertices;
		u32		numIndices;
		u64		positionOffset;
		u64		normalOffset;
		u64		texcoordOffset;
	};	// struct MeshShape

	/**********************************************//**
	 * @brief �}�e���A��
	**************************************************/
	struct MeshMaterial
	{
		char	name[64];
	};	// struct MeshMaterial

	/**********************************************//**
	 * @brief �T�u���b�V��
	**************************************************/
	struct MeshSubmesh
	{
		s32		shapeIndex;
		s32		materialIndex;
		u64		indexBufferOffset;
		u32		numSubmeshIndices;
	};	// struct MeshMaterial

	/**********************************************//**
	 * @brief ���b�V���w�b�_
	**************************************************/
	struct MeshHead
	{
		char	fourCC[4];
		s32		numShapes;
		s32		numMaterials;
		s32		numSubmeshes;
	};	// struct MeshHead

}	// namespace ym


//	EOF
