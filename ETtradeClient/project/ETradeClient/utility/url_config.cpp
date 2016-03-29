#include "stdafx.h"

#include "etradeclient/utility/url_config.h"

#include <sstream>
#include <fstream>
#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/openssl_aes_cbc.h"
#include "etradeclient/utility/string_converter.h"

static const std::string CIPHER_TEXT_FILE = "./Config/url_cfg";
static const std::string PLAIN_TEXT_FILE = CIPHER_TEXT_FILE + ".json";

static const std::string HTTP("http");
static const std::string HTTPS("https");
static const uint16_t INTERNET_DEFAULT_PORT = 0; // use the protocol-specific default
static const uint16_t INTERNET_DEFAULT_HTTP_PORT = 80;  // HTTP
static const uint16_t INTERNET_DEFAULT_HTTPS_PORT = 443; // HTTPS

URLConfig::URLConfig()
{	
	std::stringstream ss;
	try
	{
#ifdef _DEBUG
		// 在调试的时候，先生成加密文件
		AES_CBC::EncryptFileToFile(PLAIN_TEXT_FILE, CIPHER_TEXT_FILE);
#endif 
		ss << AES_CBC::DecryptFromFile(CIPHER_TEXT_FILE);
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(gbk_2_wstr(ex.what()));
		return;
	}

	namespace PT = boost::property_tree;
	try //Parse the configuration file
	{
		PT::ptree ptree;
		PT::read_json(ss, ptree);

		m_protocol = ptree.get<std::string>("protocol");
		m_is_https = boost::iequals(boost::to_lower_copy(m_protocol).c_str(), HTTPS.c_str());
		m_host = ptree.get<std::string>("host");
		
		m_port = ptree.get<uint16_t>("port");
		if (INTERNET_DEFAULT_PORT == m_port)
			m_port = m_is_https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;

		m_main_path = ptree.get<std::string>("main_path");
		m_login_path = ptree.get<std::string>("login_path");
		m_logout_path = ptree.get<std::string>("logout_path");
		m_menu_auth_path = ptree.get<std::string>("menu_auth_path");
		m_user_msg_count_path = ptree.get<std::string>("user_msg_count_path");
		m_user_msg_path = ptree.get<std::string>("user_msg_path");
		m_modify_pwd_path = ptree.get<std::string>("modify_pwd_path");
	}
	catch (...) // Catch the exception in order for logging.
	{
		LOG_ERROR(L"解析路径配置文件“./Config/url_cfg.json”时出错！请确认该配置文件存在，存放的json数据格式正确！");
		throw; // Don't swallow the exception!
	}
}

void URLConfig::ModifyCfg(bool is_https, uint16_t port)
{
	std::stringstream ss;

	m_is_https = is_https;
	m_protocol = is_https ? HTTPS : HTTP;
	m_port = port;

	namespace PT = boost::property_tree;
	try //Parse the configuration file
	{
		PT::ptree root;
		root.put("protocol", m_protocol);
		root.put("host", m_host);
		root.put("port", m_port);
		root.put("main_path", m_main_path);
		root.put("login_path", m_login_path);
		root.put("logout_path", m_logout_path);
		root.put("menu_auth_path", m_menu_auth_path);
		root.put("user_msg_count_path", m_user_msg_count_path);
		root.put("user_msg_path", m_user_msg_path);
		root.put("modify_pwd_path", m_modify_pwd_path);
		PT::write_json(ss, root, true);
	}
	catch (...) // Catch the exception in order for logging.
	{
		LOG_ERROR(L"组Json字符流时出错，请检查！");
		throw; // Don't swallow the exception!
	}

	try
	{
		AES_CBC::EncryptToFile(ss.str(), CIPHER_TEXT_FILE);

#ifdef _DEBUG // Save to plain text file under debug mode.
		std::ofstream ofs(PLAIN_TEXT_FILE, std::ios_base::out | std::ios_base::binary);
		if (!ofs.is_open())
			throw std::exception(("打开文件" + PLAIN_TEXT_FILE + "失败！").c_str());
		ofs << ss.str();
		ofs.close();
#endif
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(gbk_2_wstr(ex.what()));
	}
}

URLConfig& URLConfig::Instance()
{
	static URLConfig url_config;
	return url_config;
}

bool URLConfig::IsHttps() const
{
	return m_is_https;
}
std::string URLConfig::Host() const
{
	return m_host;
}
uint16_t URLConfig::Port() const
{
	return m_port;
}
std::string URLConfig::FullHost() const
{
	return m_protocol + "://" + m_host + ":" + std::to_string(m_port);
}
std::string URLConfig::MainPath() const
{
	return m_main_path;
}
std::string URLConfig::LoginPath() const
{
	return m_login_path;
}
std::string URLConfig::LogoutPath() const
{
	return m_logout_path;
}
std::string URLConfig::MenuAuthPath() const
{
	return m_menu_auth_path;
}
std::string URLConfig::UserMsgCountPath() const
{
	return m_user_msg_count_path;
}
std::string URLConfig::UserMsgPath() const
{
	return m_user_msg_path;
}
std::string URLConfig::PwdModificationPath() const
{
	return m_modify_pwd_path;
}