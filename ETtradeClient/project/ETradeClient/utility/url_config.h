#ifndef ETRADECLIENT_UTILITY_URL_CONFIG_H_INCLUDED
#define ETRADECLIENT_UTILITY_URL_CONFIG_H_INCLUDED

/*A "singleton" class to provide a universal interface to access the configuration about URLs.*/

#include <string>
#include <cstdint>

class URLConfig
{
public:
	/*Exception will be thrown if config file path not correct.*/
	static URLConfig& Instance();

	/*Only the protocol and server port can be modified.
	TODO: review what should be modified.*/
	void ModifyCfg(bool is_https, uint16_t port);

	// Some interfaces followed as below return string object,
	// don't worry about the cost because we have RVO & move semantics.
	bool IsHttps() const;
	std::string Host() const; // xxx.com
	uint16_t Port() const; // 443 or 80 or user configured port.
	std::string FullHost() const; // Helper function to get the full host path: protocol + host + port.
	std::string MainPath() const; // /main
	std::string LoginPath() const; // /login
	std::string LogoutPath() const;
	std::string MenuAuthPath() const;
	std::string UserMsgCountPath() const;
	std::string UserMsgPath() const;
	std::string PwdModificationPath() const;
	std::string CreateMerchantPath() const;
private:
	URLConfig();
	URLConfig(const URLConfig&);
	URLConfig& operator=(const URLConfig&);

	bool m_is_https;
	std::string m_protocol;
	std::string m_host;
	uint16_t	m_port;
	std::string m_main_path;
	std::string m_login_path;
	std::string m_logout_path;
	std::string m_menu_auth_path;
	std::string m_user_msg_count_path;
	std::string m_user_msg_path;
	std::string m_modify_pwd_path;
	std::string m_create_merchant_path;
};
#endif // ETRADECLIENT_UTILITY_URL_CONFIG_H_INCLUDED
