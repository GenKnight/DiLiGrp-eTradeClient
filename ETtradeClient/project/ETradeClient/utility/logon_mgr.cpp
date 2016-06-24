#include "stdafx.h"

#include "etradeclient/utility/logon_mgr.h"

#include "etradeclient/utility/win_http.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/string_converter.h"
#include "etradeclient/utility/session.h"

#include <boost/property_tree/ptree.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.

namespace{

	// Tag & value of the log in response JSON string.
	static const std::string JSON_TAG_CODE = "code";
	static const std::string JSON_TAG_MSG = "message";
	static const std::string JSON_VAL_SUCC = "success";
	static const std::string JSON_TAG_IS_FIRST_LOGIN = "is_first";
	static const std::string JSON_VAL_IS_FIRST_LOGIN_TRUE = "true";
	static const std::string JSON_VAL_IS_FIRST_LOGIN_FALSE = "false";

	std::string WinHttpGet(const std::string& url_path)
	{
		const uint32_t kHTTPOK = 200;
		auto& url_cfg = URLConfig::Instance();

		WinHttp win_http;
		win_http.ConnectHost(url_cfg.Host(), url_cfg.Port(), url_cfg.IsHttps());
		auto& request = win_http.OpenRequest(WinHttp::Method::GET, url_path);
		if (url_cfg.IsHttps())
		{
			auto& app_cfg = ApplicationConfig::Instance();
			request.SetClientCertificate(app_cfg.ClientCertStore(), app_cfg.ClientCertSubject());
		}
		request.SetCookies(Session::Instance().Cookies());
		request.Send();
		uint32_t status_code = request.GetResponseStatus();
		if (kHTTPOK != status_code)
		{
			std::string err_msg = "网络请求错误！ 错误码: " + std::to_string(status_code);
			throw std::exception(err_msg.c_str());
		}
		std::string response_body = request.ReadResponseBody();
		if (response_body.empty())
			throw std::exception("获取服务器响应数据失败，请确保网络连接正常");
		return response_body;
	}

}


LogonMgr& LogonMgr::Instance()
{
	static LogonMgr instance;
	return instance;
}

LogonMgr::LogonMgr() : m_err_msg(L"")
{}

bool LogonMgr::DoLogin(const std::wstring& account, const std::wstring& pwd, const std::string& type)
{
	const uint32_t kHTTPOK = 200;
	m_err_msg = L"";

	auto& url_cfg = URLConfig::Instance();
	try
	{
		WinHttp win_http;
		win_http.ConnectHost(url_cfg.Host(), url_cfg.Port(), url_cfg.IsHttps());
		auto& request = win_http.OpenRequest(WinHttp::Method::POST, url_cfg.LoginPath());
		request.SetContentType("Content-Type:application/json;charset=UTF-8");
		std::string post_data("{\"account\":\"" + wstr_2_str(account) + "\"," + "\"password\":\"" + wstr_2_str(pwd) + "\"," + "\"type\":\"" + type + "\"}");
		request.SetPostData(post_data);
		if (url_cfg.IsHttps())
		{
			auto& app_cfg = ApplicationConfig::Instance();
			request.SetClientCertificate(app_cfg.ClientCertStore(), app_cfg.ClientCertSubject());
		}
		request.Send();
		uint32_t status_code = request.GetResponseStatus();
		if (kHTTPOK != status_code)
		{
			m_err_msg = L"网络请求错误！ 错误码: " + std::to_wstring(status_code);
			return false;
		}
		std::string response_body = request.ReadResponseBody();
		if (response_body.empty())
		{
			m_err_msg = L"获取服务器响应数据失败，请确保网络连接正常！";
			return false;
		}
		if (!GetLoginResult(response_body)) // If log in failed.
			return false;
		Session::Instance().SetCookies(request.GetCookies()); // If log in succeeds, keep the cookies in session.
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(gbk_2_wstr(ex.what()));
		m_err_msg = L"发送登录请求失败，请确保网络连接正常！";
		return false;
	}
	return true;
}

bool LogonMgr::DoLogout()
{
	bool log_out_res = true;
	std::string response_body;
	try
	{
		response_body = WinHttpGet(URLConfig::Instance().LogoutPath());
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(L"网络连接有问题，退出系统失败。错误信息： " + gbk_2_wstr(ex.what()));
		return false;
	}

	namespace PT = boost::property_tree;
	try //Parse the configuration file
	{
		PT::ptree ptree;
		std::stringstream ss;
		ss << response_body;
		PT::read_json(ss, ptree);
		std::string res = ptree.get<std::string>(JSON_TAG_CODE);
		log_out_res = res.compare(JSON_VAL_SUCC) == 0 ? true : false;
		if (!log_out_res)
			LOG_ERROR(L"服务器退出处理失败，返回结果为：" + str_2_wstr(ptree.get<std::string>(JSON_TAG_MSG)));
	}
	catch (...)
	{
		LOG_ERROR(L"解析服务器返回的退出信息时出错！请确认返回数据不为空，返回的数据格式为正确的Json格式！");
		return false;
	}
	return log_out_res;
}

const std::wstring& LogonMgr::ErrorMsg() const
{
	return m_err_msg;
}

bool LogonMgr::GetLoginResult(const std::string& response_str) //Parse the response string which is a JSON string.
{
	namespace PT = boost::property_tree;
	bool login_res = false;
	bool is_first_login = false;
	try //Parse the configuration file
	{
		PT::ptree ptree;
		std::stringstream ss;
		ss << response_str;
		PT::read_json(ss, ptree);
		std::string res = ptree.get<std::string>(JSON_TAG_CODE);
		login_res = res.compare(JSON_VAL_SUCC) == 0 ? true : false;

		if (!login_res)
			m_err_msg = str_2_wstr(ptree.get<std::string>(JSON_TAG_MSG));
#ifndef _TEST
		// TODO: for test, modify this.
		std::string is_first = ptree.get<std::string>(JSON_TAG_IS_FIRST_LOGIN);
		Session::Instance().SetFirstLoginFlag(is_first.compare(JSON_VAL_IS_FIRST_LOGIN_TRUE) == 0 ? true : false);
#else
		Session::Instance().SetFirstLoginFlag(false);
#endif
	}
	catch (...)
	{
		LOG_ERROR(L"解析服务器返回的登录结果信息时出错！请确认返回数据不为空，返回的数据格式为正确的Json格式！");
		return false;
	}
	return login_res;
}

