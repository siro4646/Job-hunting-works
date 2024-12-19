
#include "shader.h"
#include "../device/device.h"
#include "../../../utility/fileStream/fileStream.h"

namespace ym
{
	//----
	bool Shader::Initialize(Device *pDev, ShaderType::Type type, const char *filename)
	{
		// �t�@�C����ǂݍ���
		File f;
		if (!f.ReadFile(filename))
		{
			return false;
		}

		// ������
		return Initialize(pDev, type, f.GetData(), f.GetSize());
	}

	//----
	bool Shader::Initialize(Device *pDev, ShaderType::Type type, const void *pData, size_t size)
	{
		if (!pData || !size)
		{
			return false;
		}

		// ���������m��
		pData_ = new u8[size];
		if (!pData_)
		{
			return false;
		}

		// �R�s�[
		memcpy(pData_, pData, size);

		size_ = size;
		shaderType_ = type;

		return true;
	}

	//----
	void Shader::Destroy()
	{
		ym::SafeDeleteArray(pData_);
	}

}	// namespace ym