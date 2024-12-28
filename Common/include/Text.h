#pragma once
#include <Windows.h>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#ifndef __EZUI__STRING
#define __TEXT__STRING

#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#define	 UI_VAR_EXPORT UI_EXPORT
#else
#define UI_EXPORT 
#define UI_VAR_EXPORT __declspec(dllimport)
#endif // _WINDLL

//----------------------------------------------------------------
namespace Text {
	/// <summary>
	/// utf8字符串
	/// </summary>
	class UI_EXPORT String :public std::string {
	public:
		String();
		virtual ~String();
		String(const String& _right)noexcept;
		String(String&& _right)noexcept;
		String& operator=(const String& _right)noexcept;
		String& operator=(String&& _right)noexcept;
		String(const std::string& str)noexcept;
		String(const char* szbuf)noexcept;
		String(const wchar_t* szbuf)noexcept;
		String(const std::wstring& wstr)noexcept;
		//the utf8 Length
		virtual size_t length() const final;
		std::wstring unicode() const;
		std::string ansi() const;
		void erase(char _ch);
		void erase(size_t pos, size_t count);
		String replace(char oldChar, char newChar);
		String replace(const String& oldText, const String& newText, bool allReplace = true)const;
		String toLower()const;
		String toUpper()const;
		std::vector<String> split(const String& ch)const;
		bool operator==(const wchar_t* szbuf)const;
		bool operator==(const std::wstring& wStr)const;
	};

	//base convert
	UI_EXPORT void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr);
	UI_EXPORT void UnicodeToAny(const std::wstring& unicode_wstr, UINT codePage, std::string* out_str);
	//
	UI_EXPORT void GBKToUTF8(const std::string& str, std::string* outStr);
	UI_EXPORT void UTF8ToGBK(const std::string& str, std::string* outStr);
	UI_EXPORT void ANSIToUniCode(const std::string& str, std::wstring* outStr);
	UI_EXPORT void ANSIToUTF8(const std::string& str, std::string* outStr);
	UI_EXPORT void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
	UI_EXPORT void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
	UI_EXPORT void UTF8ToANSI(const std::string& str, std::string* outStr);
	UI_EXPORT void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
	//
	UI_EXPORT void Tolower(std::string* str_in_out);
	UI_EXPORT void Toupper(std::string* str_in_out);
	UI_EXPORT void Erase(std::string* str_in_out, char ch);
	UI_EXPORT void Replace(std::string* str_in_out, char oldChar, char newChar);
	UI_EXPORT void Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll = true);
	UI_EXPORT void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);
	//
	UI_EXPORT String ToString(double number, int keepBitSize);
};
//----------------------------------------------------------------
#endif // !__EZUI__STRING
namespace EzUI {
	using EString = ::Text::String;
};
