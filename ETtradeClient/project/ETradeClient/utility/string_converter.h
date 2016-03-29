#ifndef ETRADECLIENT_UTILITY_STRING_CONVERTER_H_INCLUDED
#define ETRADECLIENT_UTILITY_STRING_CONVERTER_H_INCLUDED

#include <string>

/*These APIs can be used to make conversion between Unicode string and UTF-8 string.
For example:
std::wstring ws = L"ÖÐÎÄ"; // 'ws' is Unicode string with value of '0x4e2d' & '0x6587'.
std::string s = wstr_2_str(ws); // 's' is UTF-8 string with value of '0xe4b8ad' & '0xe69687'.
std::wstring ws_ = str_2_wstr(s); // 'ws_' is Unicode string with value of '0x4e2d' & '0x6587'.
*/

std::string wstr_2_str(const std::wstring& wstr);
std::string wstr_2_str(const wchar_t* wstr);
std::string wstr_2_str(const wchar_t* first, const wchar_t* last);

std::wstring str_2_wstr(const std::string& str);
std::wstring str_2_wstr(const char* str);
std::wstring str_2_wstr(const char* first, const char* last);

/*This API is specially used to do conversion between Chinese characters in GBK encoding & Unicode encoding.*/
std::wstring gbk_2_wstr(const std::string& gbk_str);
std::string wstr_2_gbk(const std::wstring& wstr);

#endif // ETRADECLIENT_UTILITY_STRING_CONVERTER_H_INCLUDED
