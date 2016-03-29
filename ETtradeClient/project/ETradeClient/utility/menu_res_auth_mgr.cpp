#include "stdafx.h"

#include "etradeclient/utility/menu_res_auth_mgr.h"

#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "etradeclient/browser/session.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/string_converter.h"
#include "etradeclient/utility/openssl_aes_cbc.h"
#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/win_http.h"
#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.

namespace PT = boost::property_tree;
namespace fs = boost::filesystem;

struct MenuAuthItem
{
	uint32_t				id;// Menu Auth ID, defined by server side. If not authorized by server, value is 0.
	std::string				url_path; // Defined by server side. If not authorized by server, value is "".
	std::wstring			remark; // Defined by server side. If not authorized by server, value is "".
};

bool MenuResAuthMgr::UpdateAuth()
{
	try
	{
		ReadMenuResAuthCfg(); // 1st, read menu res id and auth id config from config file.
		DoUpdateAuth(RequestAuthInfo()); // Then request auth info from server and update the authorization of every menu.
	}
	catch (std::exception& ex)
	{
		LOG_FATAL(L"获取用户权限失败: " + gbk_2_wstr(ex.what()));
		return false;
	}
	return true;
}

const MenuResAuthMgr::MenuItemsType& MenuResAuthMgr::MenuItems() const
{
	return m_menu_items;
}

void MenuResAuthMgr::ReadMenuResAuthCfg()
{
	// For those menu item which are not configured by server side, we call them local menu resources,
	// and we define their "auth_id" as 0 in config file. These menu items are authorized by default("is_authorized == true").
	// For those menu items whose "auth_id" are not defined as 0 in config file,
	// their authorization needs to be configured by server side, thus they are unauthorized by default("is_authorized == false").
	static const uint32_t LOCAL_MENU_RES_AUTH_ID = 0;

	try
	{
		const std::string kMenuBarConfEncryptFile("./Config/menu_res_auth_cfg");

#ifdef _DEBUG
		// 在调试的时候，先生成加密文件
		const std::string kMenuBarConfJsonFile("./Config/menu_res_auth_cfg.json");
		AES_CBC::EncryptFileToFile(kMenuBarConfJsonFile, kMenuBarConfEncryptFile);
#endif 

		std::stringstream ss;
		ss << AES_CBC::DecryptFromFile(kMenuBarConfEncryptFile);

		//Parse the configuration file
		PT::ptree ptree;
		PT::read_json(ss, ptree);
		for (const auto& elem : ptree)
		{
			MenuItem menu_item;
			menu_item.icon_name = elem.second.get<std::string>("icon");
			menu_item.auth_id = elem.second.get<uint32_t>("auth_id");
			menu_item.is_authorized = (menu_item.auth_id == LOCAL_MENU_RES_AUTH_ID) ? true : false;
			menu_item.url_path = ""; // Empty by default.
			menu_item.remark = L""; // Empty by default.
			PT::ptree menu_index_list = elem.second.get_child("index");
			for (const auto& elem_ : menu_index_list)
			{
				menu_item.index_list.emplace_back(elem_.second.get_value<uint32_t>());
			}
			m_menu_items.emplace(std::make_pair(elem.second.get<uint32_t>("res_id"), menu_item));
		}
	}
	catch (...)
	{
		throw std::exception("解析菜单资源配置文件出错！");
	}
}

std::string MenuResAuthMgr::RequestAuthInfo() const
{
	std::string auth_url = URLConfig::Instance().MenuAuthPath();
	const uint32_t kHTTPOK = 200;
	auto& url_cfg = URLConfig::Instance();

	WinHttp win_http;
	win_http.ConnectHost(url_cfg.Host(), url_cfg.Port(), url_cfg.IsHttps());
	auto& request = win_http.OpenRequest(WinHttp::Method::GET, auth_url);
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

void MenuResAuthMgr::DoUpdateAuth(const std::string& auth_data)
{
	std::vector<MenuAuthItem> menu_auth_items;
	try // Parse the auth data which is JSON format.
	{
		PT::ptree ptree;
		std::stringstream ss; // boost::property_tree::read_json ask for non-const stringstream, don't ask me why, boost asks for this.
		ss << auth_data;
		PT::read_json(ss, ptree);

		for (const auto& elem : ptree)
		{
			MenuAuthItem auth_item;
			auth_item.id = elem.second.get<uint32_t>("id");
			auth_item.url_path = elem.second.get<std::string>("url_path");
			auth_item.remark = str_2_wstr(elem.second.get<std::string>("remark"));
			menu_auth_items.emplace_back(auth_item);
		}
	}
	catch (...)
	{
		throw std::exception("解析服务器返回的授权菜单资源信息时出错！请确认返回数据不为空，返回的数据格式为正确的Json格式！");
	}

	for (auto& auth_item : menu_auth_items)
	{
		MenuItemsType::iterator it = std::find_if(m_menu_items.begin(), m_menu_items.end(),
			[&auth_item](MenuItemsType::value_type& x)
			{
				return x.second.auth_id == auth_item.id;
			});
		if (it != m_menu_items.end()) // If authorized, update its information.
		{
			it->second.is_authorized = true;
			it->second.url_path = auth_item.url_path;
			it->second.remark = auth_item.remark;
		}
	}
}