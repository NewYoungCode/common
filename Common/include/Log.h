#pragma once
#include "FileSystem.h"
#include "Time.hpp"
#include <mutex>
namespace Log {
	//是否启用日志
	extern bool Enable;
	extern std::mutex __logMtx;
	extern void WriteLog(const Text::Utf8String& log);

	template<typename ...T>
	/// <summary>
	/// 打印utf8的字符
	/// </summary>
	/// <typeparam name="...T"></typeparam>
	/// <param name="formatStr"></param>
	/// <param name="...args"></param>
	inline void Info(const Text::Utf8String& formatStr,const T &...args) {
		if (!Enable)return;
		__logMtx.lock();
		int size = 1024 * 1024 * 5;//5M的内存
		char* buf = new char[size] { 0 };
		auto count = sprintf_s((buf), size, formatStr.c_str(), std::forward<const T&>(args)...);
		buf[count] = '\n';
		buf[count + 1] = 0;
		Text::Utf8String info(buf);
		info = Time::Now::ToString("hh:mm:ss ") + info;
		delete[] buf;
		auto wstr = info.utf16();
		std::wcout << wstr;
		OutputDebugStringW(wstr.c_str());
		WriteLog(info);
		__logMtx.unlock();
	}
};
