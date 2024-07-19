#pragma once
#include "Text.h"
//定义................................................
namespace Text {
	size_t String::length() const {
		auto* p = this->c_str();
		size_t pos = 0, count = 0;
		while (p[pos] && pos < this->size()) {
			if ((p[pos] & 0xc0) != 0x80) {
				++count;
			}
			++pos;
		}
		return count;
	}
	String::String() {}
	String::~String()
	{
	}
	String::String(const std::string& str)noexcept :std::string(str) {}
	String::String(const char* szbuf)noexcept :std::string(szbuf) {}
	String::String(const wchar_t* szbuf)noexcept {
		if (szbuf == NULL)return;
		Text::UnicodeToUTF8(szbuf, this);
	}
	String::String(const std::wstring& wstr)noexcept {
		Text::UnicodeToUTF8(wstr, this);
	}
	std::vector<std::string> String::split(const String& ch_)const {
		std::vector<std::string> arr;
		Text::Split(*this, ch_, &arr);
		return arr;
	}
	void String::erase(char _ch)
	{
		Text::Erase(this, _ch);
	}
	void String::erase(size_t pos, size_t count)
	{
		__super::erase(pos, count);
	}
	String String::replace(char oldChar, char newChar)
	{
		String newStr = *this;
		Text::Replace(&newStr, oldChar, newChar);
		return newStr;
	}
	String String::replace(const String& oldText, const String& newText, bool allReplace) const
	{
		String newStr = *this;
		Text::Replace(&newStr, oldText, newText, allReplace);
		return newStr;
	}
	String String::toLower() const
	{
		String str(*this);
		Text::Tolower(&str);
		return str;
	}
	String String::toUpper() const
	{
		String str(*this);
		Text::Toupper(&str);
		return str;
	}
	bool String::operator==(const wchar_t* szbuf)const
	{
		std::string u8str;
		Text::UnicodeToUTF8(szbuf, &u8str);
		return (*this == u8str);
	}
	bool String::operator==(const std::wstring& wStr)const
	{
		std::string u8str;
		Text::UnicodeToUTF8(wStr, &u8str);
		return (*this == u8str);
	}
	std::wstring String::unicode() const {
		std::wstring wstr;
		Text::UTF8ToUnicode(*this, &wstr);
		return wstr;
	}
	std::string String::ansi() const {
		std::string str;
		Text::UTF8ToANSI(*this, &str);
		return str;
	}




	void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr) {
		std::wstring& wstrCmd = *out_wstr;
		int bytes = ::MultiByteToWideChar(codePage, 0, src_str.c_str(), src_str.size(), NULL, 0);
		wstrCmd.resize(bytes);
		bytes = ::MultiByteToWideChar(codePage, 0, src_str.c_str(), src_str.size(), const_cast<wchar_t*>(wstrCmd.c_str()), wstrCmd.size());
	}
	void UnicodeToAny(const std::wstring& wstr, UINT codePage, std::string* out_str) {
		std::string& strCmd = *out_str;
		int bytes = ::WideCharToMultiByte(codePage, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		strCmd.resize(bytes);
		bytes = ::WideCharToMultiByte(codePage, 0, wstr.c_str(), wstr.size(), const_cast<char*>(strCmd.c_str()), strCmd.size(), NULL, NULL);
	}
	void ANSIToUniCode(const std::string& str, std::wstring* outStr)
	{
		AnyToUnicode(str, ::GetACP(), outStr);
	}
	void UnicodeToANSI(const std::wstring& wstr, std::string* outStr)
	{
		UnicodeToAny(wstr, ::GetACP(), outStr);
	}
	void GBKToUTF8(const std::string& str, std::string* outStr) {
		const int gbkCodePage = 936;
		std::wstring wstr;
		AnyToUnicode(str, gbkCodePage, &wstr);
		UnicodeToUTF8(wstr, outStr);
	}
	void UTF8ToGBK(const std::string& str, std::string* outStr) {
		const int gbkCodePage = 936;
		std::wstring wstr;
		UTF8ToUnicode(str, &wstr);
		UnicodeToAny(wstr, gbkCodePage, outStr);
	}
	void ANSIToUTF8(const std::string& str, std::string* outStr)
	{
		UINT codePage = ::GetACP();
		if (codePage == CP_UTF8) {
			*outStr = str;//如果本身就是utf8则不需要转换
			return;
		}
		std::wstring wstr;
		AnyToUnicode(str, codePage, &wstr);
		UnicodeToUTF8(wstr, outStr);
	}
	void UTF8ToANSI(const std::string& str, std::string* outStr) {
		UINT codePage = ::GetACP();
		if (codePage == CP_UTF8) {
			*outStr = str;//如果本身就是utf8则不需要转换
			return;
		}
		std::wstring wstr;
		UTF8ToUnicode(str, &wstr);
		UnicodeToAny(wstr, codePage, outStr);
	}
	void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr)
	{
		UnicodeToAny(wstr, CP_UTF8, outStr);
	}
	void UTF8ToUnicode(const std::string& str, std::wstring* outStr) {
		AnyToUnicode(str, CP_UTF8, outStr);
	}
	void Tolower(std::string* str_in_out)
	{
		std::string& str = *str_in_out;
		for (size_t i = 0; i < str.size(); ++i)
		{
			char& ch = (char&)str.c_str()[i];
			if (ch >= 65 && ch <= 90) {
				ch += 32;
			}
		}
	}
	void Toupper(std::string* str_in_out)
	{
		std::string& str = *str_in_out;
		for (size_t i = 0; i < str.size(); ++i)
		{
			char& ch = (char&)str.c_str()[i];
			if (ch >= 97 && ch <= 122) {
				ch -= 32;
			}
		}
	}
	void Erase(std::string* str_in_out, const char& _char) {
		const String& self = *str_in_out;
		char* bufStr = new char[self.size() + 1] { 0 };
		size_t pos = 0;
		for (auto& it : self) {
			if (_char == it)continue;
			bufStr[pos] = it;
			++pos;
		}
		*str_in_out = bufStr;
		delete[] bufStr;
	}
	void Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll)
	{
		std::string& newStr = *str_in_out;
		size_t pos;
		pos = newStr.find(oldText);
		while (pos != std::string::npos)
		{
			newStr.replace(pos, oldText.size(), newText);
			pos = newStr.find(oldText);
			if (!replaceAll) {
				break;
			}
		}
	}
	void Replace(std::string* str_in_out, char oldChar, char newChar)
	{
		for (auto& it : *str_in_out) {

			if (it == oldChar) {
				it = newChar;
			}
		}
	}
	void Split(const std::string& str_in, const std::string& ch_, std::vector<std::string>* strs_out) {

		std::vector<std::string>& arr = *strs_out;
		arr.clear();
		if (str_in.empty()) return;

		std::string buf = str_in;
		size_t pos = buf.find(ch_);
		if (pos == std::string::npos) {
			arr.push_back(buf);
			return;
		}
		while (pos != std::string::npos) {
			auto item = buf.substr(0, pos);
			if (!item.empty()) {
				arr.push_back(item);
			}
			buf = buf.erase(0, pos + ch_.size());
			pos = buf.find(ch_);
			if (pos == std::string::npos) {
				if (!buf.empty()) {
					arr.push_back(buf);
				}
			}
		}
	}

	String  ToString(double number, int keepBitSize) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(keepBitSize) << number;
		return oss.str();
	}

};