#include "stdafx.h"

#include "etradeclient/utility/win_http.h"

#include <memory>
#include <vector>
#include <sstream>
#include <cctype>
#include <regex>
#include <boost/algorithm/string.hpp>

#include "wincrypt.h"

#include "etradeclient/utility/string_converter.h"

namespace
{
	typedef std::vector<std::pair<std::wstring, std::wstring>> CookieItems;
	std::wstring GetCookieItem(CookieItems::const_iterator first, CookieItems::const_iterator last, const std::wstring& item)
	{
		auto it = std::find_if(first, last,
			[&](const CookieItems::value_type& elem)
			{
				return boost::iequals(elem.first, item);
			});
		if (it != last)
			return it->second;
		else
			return L"";
	}
}

WinHttp::WinHttp() : m_is_https(false), m_session(nullptr), m_connect(nullptr)
{
	// Use WinHttpOpen to obtain a session handle, all the subsequent request are based on this session context.
	m_session = WinHttpOpen(L"WinHTTP Request", // This param can be any string, even empty string.
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!m_session)
	{
		std::string err_msg = "获取WinHTTP会话句柄失败！错误代码：" + std::to_string(GetLastError());
		throw std::exception(err_msg.c_str());
	}
}

WinHttp::~WinHttp()
{
	// Close all handles so that all resources are released.
	DisconnectHost();
	if (m_session)
	{
		WinHttpCloseHandle(m_session);
		m_session = nullptr;
	}
}

void WinHttp::ConnectHost(const std::string& host, uint16_t port, bool is_https)
{
	m_is_https = is_https;
	m_connect = WinHttpConnect(m_session, str_2_wstr(host).c_str(), port, 0);
	if (!m_connect)
	{
		std::string err_msg = "建立WinHttp连接失败！错误代码：" + std::to_string(GetLastError());
		throw std::exception(err_msg.c_str());
	}
}

void WinHttp::DisconnectHost()
{
	if (m_connect)
	{
		WinHttpCloseHandle(m_connect);
		m_connect = nullptr;
	}
}

WinHttp::Request WinHttp::OpenRequest(const Method& method, const std::string& res_path)
{
	std::wstring method_name;
	switch (method)
	{
	case Method::GET:
		method_name = L"GET";
		break;
	case Method::POST:
		method_name = L"POST";
		break;
	}
	// Create an HTTP request handle.
	HINTERNET handle = WinHttpOpenRequest(m_connect, method_name.c_str(), str_2_wstr(res_path).c_str(),
		NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, m_is_https ? WINHTTP_FLAG_SECURE : 0);
	if (!handle)
	{
		std::string err_msg = "创建WinHttp请求失败！错误代码：" + std::to_string(GetLastError());
		throw std::exception(err_msg.c_str());
	}
	return Request(handle, method);
}

void WinHttp::CloseRequest(Request& req)
{
	req.Close();
}

//--------------------------------------------------------------------------

WinHttp::Request::Request(Request&& rhs)
{
	m_request = std::move(rhs.m_request);
	rhs.m_request = NULL;
	m_method = rhs.m_method;
	m_content_type = std::move(rhs.m_content_type);
	m_post_data = std::move(rhs.m_post_data);
}

WinHttp::Request& WinHttp::Request::operator=(Request&& rhs)
{
	if (this != &rhs)
	{
		m_request = std::move(rhs.m_request);
		rhs.m_request = NULL;
		m_method = rhs.m_method;
		m_content_type = std::move(rhs.m_content_type);
		m_post_data = std::move(rhs.m_post_data);
	}
	return *this;
}

WinHttp::Request::Request(HINTERNET handle, const WinHttp::Method& method) : m_request(handle), m_method(method)
{}

WinHttp::Request::~Request()
{
	Close();
}

void WinHttp::Request::SetCookies(const Cookies& cookies)
{
	std::stringstream cookie_str;
	cookie_str << "Cookie: ";
	for (const auto& cookie : cookies)
	{
		cookie_str << cookie.name << "=" << cookie.value << "; ";
		cookie_str << "domain: " << cookie.domain << "; ";
		cookie_str << "httponly: " << cookie.httponly << "; ";
		cookie_str << "path: " << cookie.path << "; ";
		cookie_str << "secure: " << cookie.secure << "; ";
	}
	WinHttpAddRequestHeaders(m_request, str_2_wstr(cookie_str.str()).c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
}

void WinHttp::Request::SetClientCertificate(const std::string& cert_store_name, const std::string& subject_name)
{
	HCERTSTORE cert_store = CertOpenSystemStore(0, str_2_wstr(cert_store_name).c_str());
	if (cert_store)
	{
		PCCERT_CONTEXT cert_context = CertFindCertificateInStore(cert_store,
			X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			0,
			CERT_FIND_SUBJECT_STR,
			str_2_wstr(subject_name).c_str(), //Issuer string in the certificate.
			NULL);
		if (cert_context)
		{
			WinHttpSetOption(m_request,
				WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
				(LPVOID)cert_context,
				sizeof(CERT_CONTEXT));
			CertFreeCertificateContext(cert_context);
		}
		else
		{
			std::string err_msg("查找证书失败！错误代码：" + std::to_string(GetLastError()));
			throw std::exception(err_msg.c_str());
		}
		CertCloseStore(cert_store, 0);
	}
	else
	{
		std::string err_msg("打开认证存储区域失败！错误代码：" + std::to_string(GetLastError()));
		throw std::exception(err_msg.c_str());
	}
}

void WinHttp::Request::SetContentType(std::string content_type)
{
	m_content_type = content_type;
}

void WinHttp::Request::SetPostData(const std::string& post_data)
{
	m_post_data = post_data;
}

void WinHttp::Request::Send()
{
	switch (m_method)
	{
	case Method::GET:
		Get();
		break;
	case Method::POST:
		Post();
		break;
	default:
		return;
	}
	// Read the response data.
	if (TRUE != WinHttpReceiveResponse(m_request, NULL)) // Avoid warning C4800: 'BOOL' : forcing value to bool 'true' or 'false
	{
		std::string err_msg("接收WinHttp响应数据失败！错误代码：" + std::to_string(GetLastError()));
		throw std::exception(err_msg.c_str());
	}
}

std::string WinHttp::Request::ReadResponseHeader() const
{
	std::wstring header(L"");
	QuerySingleHeader(header, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_NO_HEADER_INDEX);
	return wstr_2_str(header);
}

std::string WinHttp::Request::ReadResponseBody() const
{
	DWORD data_size = 0;
	DWORD num_of_bytes_read = 0;
	std::string response_data("");
	do
	{
		data_size = 0;
		if (!WinHttpQueryDataAvailable(m_request, &data_size)) // Check for available data.
		{
			std::string err_msg("获取WinHttp响应数据失败！错误代码：" + std::to_string(GetLastError()));
			throw std::exception(err_msg.c_str());
		}
		if (data_size > 0)
		{
			std::unique_ptr<char[]> buffer(new char[data_size + 1]);
			if (nullptr == buffer)
			{
				std::string err_msg("读取WinHttp响应数据时内存分配失败！错误代码：" + std::to_string(GetLastError()));
				throw std::exception(err_msg.c_str());
			}
			else
			{
				// Read the data.
				ZeroMemory(buffer.get(), data_size + 1);
				if (!WinHttpReadData(m_request, buffer.get(), data_size, &num_of_bytes_read))
				{
					std::string err_msg("读取WinHttp响应数据失败！错误代码：" + std::to_string(GetLastError()));
					throw std::exception(err_msg.c_str());
				}
				response_data.append(buffer.get(), data_size);
			}
			buffer.reset(nullptr); // Release buffer memory.
		}
	} while (data_size > 0);
	return response_data;
}

uint32_t WinHttp::Request::GetResponseStatus() const
{
	std::wstring header;
	QuerySingleHeader(header, WINHTTP_QUERY_STATUS_CODE, WINHTTP_NO_HEADER_INDEX);
	return header.empty() ? 0 : stoi(header);
}

WinHttp::Cookies WinHttp::Request::GetCookies() const
{
	Cookies cookies;
	std::list<std::wstring> header_list;
	QueryMultiHeader(header_list, WINHTTP_QUERY_SET_COOKIE);

	if (!header_list.empty())
		ParseCookies(header_list, cookies);
	return cookies;
}

void WinHttp::Request::Get()
{
	if (!WinHttpSendRequest(m_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
	{
		std::string err_msg("发送WinHttp Get请求失败！错误代码：" + std::to_string(GetLastError()));
		throw std::exception(err_msg.c_str());
	}
}

void WinHttp::Request::Post()
{
	// Send a request.
	if (!WinHttpSendRequest(m_request,
		//L"Content-Type: application/x-www-form-urlencoded; charset=UTF-8", // headers.
		str_2_wstr(m_content_type).c_str(),
		-1,
		const_cast<char*>(m_post_data.c_str()),
		m_post_data.size(),
		m_post_data.size(),
		0))
	{
		std::string err_msg("发送WinHttp Post请求失败！错误代码：" + std::to_string(GetLastError()));
		throw std::exception(err_msg.c_str());
	}
}

void WinHttp::Request::QuerySingleHeader(std::wstring& header, uint32_t query_info_flag, DWORD* header_idx) const
{
	DWORD buff_len = 0;
	WinHttpQueryHeaders(m_request, query_info_flag, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &buff_len, header_idx);

	// Allocate memory for the buffer.
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && buff_len > 0)
	{
		std::unique_ptr<wchar_t[]> buffer(new wchar_t[buff_len / 2 + 1]);
		if (nullptr != buffer)
		{
			ZeroMemory(buffer.get(), buff_len / 2 + 1);
			// Now, use WinHttpQueryHeaders to retrieve the header.
			if (WinHttpQueryHeaders(m_request, query_info_flag, WINHTTP_HEADER_NAME_BY_INDEX, buffer.get(), &buff_len, header_idx))
				header.assign(buffer.get());
			else
			{
				std::string err_msg("读取WinHttp Header数据失败！错误代码：" + std::to_string(GetLastError()));
				throw std::exception(err_msg.c_str());
			}
		}
	}
}

void WinHttp::Request::QueryMultiHeader(std::list<std::wstring>& header_list, uint32_t query_info_flag) const
{
	DWORD header_idx = 0;
	while (true)
	{
		std::wstring header(L"");
		QuerySingleHeader(header, query_info_flag, &header_idx);
		if (header.empty())
			break; // No more header.
		else
			header_list.emplace_back(std::move(header));
	}
}

void WinHttp::Request::ParseCookies(const std::list<std::wstring>& header_list, Cookies& cookies) const
{
	const std::wregex reg_exp(L"(\\w+)=([^;]*)");
	for (const std::wstring& item : header_list)
	{
		CookieItems cookie_items;
		std::regex_iterator<std::wstring::const_iterator> r_cit(item.cbegin(), item.cend(), reg_exp);
		std::regex_iterator<std::wstring::const_iterator> r_cend;

		while (r_cit != r_cend)
		{
			// WARNING!!! Order is important here, because we assume the first item of a cookie is always the "name=value" pair.
			// That's why we use std::vector & emplace_back here instead of the "std::map".
			cookie_items.emplace_back((*r_cit)[1], (*r_cit)[2]/*value*/); 
			++r_cit;
		}

		if (!cookie_items.empty())
		{
			WinHttp::Cookie cookie;
			// WARNING!!! Order is important here, because we assume the first item of a cookie is always the "name=value" pair.
			auto cbegin = cookie_items.cbegin();
			cookie.name = wstr_2_str(cbegin->first);
			cookie.value = wstr_2_str(cbegin->second);
			++cbegin;

			cookie.domain = wstr_2_str(GetCookieItem(cbegin, cookie_items.cend(), L"domain"));
			cookie.path = wstr_2_str(GetCookieItem(cbegin, cookie_items.cend(), L"path"));
			cookie.secure = boost::iequals(GetCookieItem(cbegin, cookie_items.cend(), L"secure"), L"true");
			cookie.httponly = boost::iequals(GetCookieItem(cbegin, cookie_items.cend(), L"httponly"), L"true");
			cookies.emplace_back(std::move(cookie));
		}
	}
}

void WinHttp::Request::Close()
{
	if (m_request)
	{
		WinHttpCloseHandle(m_request);
		m_request = nullptr;
	}
}