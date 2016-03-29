#include "stdafx.h"

#include "etradeclient/utility/string_converter.h"

#include <locale>
#include <codecvt>

typedef std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_converter_t;
static utf8_converter_t utf8_converter;

typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt; // Convert Unicode & GBK
static std::wstring_convert<cvt> gbk_converter(new cvt("CHS")); // Chinese (Simplified)_People's Republic of China.936

std::string wstr_2_str(const std::wstring& wstr)
{
	return utf8_converter.to_bytes(wstr);
}
std::string wstr_2_str(const wchar_t* wstr)
{
	return utf8_converter.to_bytes(wstr);
}
std::string wstr_2_str(const wchar_t* first, const wchar_t* last)
{
	return utf8_converter.to_bytes(first, last);
}

std::wstring str_2_wstr(const std::string& str)
{
	return utf8_converter.from_bytes(str);
}
std::wstring str_2_wstr(const char* str)
{
	return utf8_converter.from_bytes(str);
}
std::wstring str_2_wstr(const char* first, const char* last)
{
	return utf8_converter.from_bytes(first, last);
}

std::wstring gbk_2_wstr(const std::string& gbk_str)
{
	return gbk_converter.from_bytes(gbk_str);
}

std::string wstr_2_gbk(const std::wstring& wstr)
{
	return gbk_converter.to_bytes(wstr);
}