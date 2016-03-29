#include "stdafx.h"

#include "etradeclient/utility/application_config.h"

#include <sstream>
#include <exception>

#include <boost/property_tree/ptree.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/openssl_aes_cbc.h"
#include "etradeclient/utility/string_converter.h"

static const std::string CIPHER_TEXT_FILE = "./Config/application_config";
static const std::string PLAIN_TEXT_FILE = CIPHER_TEXT_FILE + ".json";

ApplicationConfig::ApplicationConfig()
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

		m_msg_count_query_interval = ptree.get<uint32_t>("msg_count_query_interval");
		m_pwd_machine_ip = ptree.get<std::string>("pwd_machine_ip");
		m_pwd_machine_port = ptree.get<uint32_t>("pwd_machine_port");
		m_pwd_machine_timeout = ptree.get<uint32_t>("pwd_machine_timeout");
		m_pin_pad_pwd_len = ptree.get<uint8_t>("pin_pad_pwd_len");
		m_pin_pad_timeout = ptree.get<uint8_t>("pin_pad_timeout");
		m_client_cert_store = ptree.get<std::string>("client_cert_store");
		m_client_cert_subject = ptree.get<std::string>("client_cert_subject");
	}
	catch (...) // Catch the exception in order for logging.
	{
		LOG_ERROR(L"解析路径配置文件" + str_2_wstr(CIPHER_TEXT_FILE) + L"出错！请确认该配置文件存在，存放的json数据格式正确！");
		throw; // Don't swallow the exception!
	}
}

void ApplicationConfig::ModifyCfg(uint32_t			msg_query_interval,
								const std::string&	pwd_machine_ip,
								uint32_t			pwd_machine_port,
								uint32_t			pwd_machine_time_out,
								uint8_t				pin_pad_pwd_len,
								uint8_t				pin_pad_timeout)
{
	std::stringstream ss;
	namespace PT = boost::property_tree;

	m_msg_count_query_interval = msg_query_interval;
	m_pwd_machine_ip = pwd_machine_ip;
	m_pwd_machine_port = pwd_machine_port;
	m_pwd_machine_timeout = pwd_machine_time_out;
	m_pin_pad_pwd_len = pin_pad_pwd_len;
	m_pin_pad_timeout = pin_pad_timeout;
	try
	{
		PT::ptree root;
		root.put("msg_count_query_interval", m_msg_count_query_interval);
		root.put("pwd_machine_ip", m_pwd_machine_ip);
		root.put("pwd_machine_port", m_pwd_machine_port);
		root.put("pwd_machine_timeout", m_pwd_machine_timeout);
		root.put("pin_pad_pwd_len", m_pin_pad_pwd_len);
		root.put("pin_pad_timeout", m_pin_pad_timeout);
		root.put("client_cert_store", m_client_cert_store);
		root.put("client_cert_subject", m_client_cert_subject);

		PT::write_json(ss, root, true);
	}
	catch (...)
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

ApplicationConfig& ApplicationConfig::Instance()
{
	static ApplicationConfig application_config;
	return application_config;
}

uint32_t ApplicationConfig::MsgCountQueryInterval() const
{
	return m_msg_count_query_interval;
}
std::string ApplicationConfig::PWDMachineIP() const
{
	return m_pwd_machine_ip;
}
uint32_t ApplicationConfig::PWDMachinePort() const
{
	return m_pwd_machine_port;
}
uint32_t ApplicationConfig::PWDMachineTimeout() const
{
	return m_pwd_machine_timeout;
}
uint8_t ApplicationConfig::PINPadPWDLen() const
{
	return m_pin_pad_pwd_len;
}
uint8_t ApplicationConfig::PINPadTimeout() const
{
	return m_pin_pad_timeout;
}
std::string ApplicationConfig::ClientCertStore() const
{
	return m_client_cert_store;
}

std::string ApplicationConfig::ClientCertSubject() const
{
	return m_client_cert_subject;
}