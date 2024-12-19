#pragma once

#include <string>
#include <cctype>
#include <algorithm>

namespace ym
{

	inline std::string GetExtent(const std::string &name)
	{
		std::string ret;
		auto pos = name.rfind('.');
		if (pos == std::string::npos)
		{
			return ret;
		}

		ret = name.substr(pos);
		std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
		return ret;
	}

	inline std::string ConvertYenToSlash(const std::string &path)
	{
		std::string ret = path;
		std::transform(ret.begin(), ret.end(), ret.begin(), [](char c) { return (c == '\\') ? '/' : c; });
		return ret;
	}

	inline std::string GetFileName(const std::string &path)
	{
		std::string ret = ConvertYenToSlash(path);
		auto pos = ret.rfind('/');
		if (pos != std::string::npos)
		{
			ret = ret.substr(pos + 1);
		}
		return ret;
	}

	inline std::string GetFileNameWithoutExtent(const std::string &path)
	{
		std::string ret = GetFileName(path);
		auto pos = ret.rfind('.');
		if (pos != std::string::npos)
		{
			ret = ret.erase(pos);
		}
		return ret;
	}

	inline std::string GetFilePath(const std::string &path)
	{
		std::string ret = ConvertYenToSlash(path);
		auto pos = ret.rfind('/');
		if (pos != std::string::npos)
		{
			ret = ret.substr(0, pos + 1);
		}
		else
		{
			ret = "./";
		}
		return ret;
	}
	inline std::wstring Utf8ToUtf16(const std::string &utf8)
	{
		if (utf8.empty()) return std::wstring();

		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), NULL, 0);
		std::wstring utf16(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &utf16[0], sizeNeeded);
		return utf16;
	}
	inline std::string Utf16ToUtf8(const std::wstring &utf16)
	{
		if (utf16.empty()) return std::string();

		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), (int)utf16.size(), NULL, 0, NULL, NULL);
		std::string utf8(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), (int)utf16.size(), &utf8[0], sizeNeeded, NULL, NULL);
		return utf8;
	}

}	// namespace ym


//	EOF
