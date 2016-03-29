#ifndef ETRADECLIENT_UTILITY_APPLICATION_CONFIG_H_INCLUDED
#define ETRADECLIENT_UTILITY_APPLICATION_CONFIG_H_INCLUDED

/*A "singleton" class to provide a universal interface to access the configuration of application.*/
#include <cstdint>
#include <string>

class ApplicationConfig
{
public:
	/*Exception will be thrown if config file path not correct.*/
	static ApplicationConfig& Instance();

	void ModifyCfg(	uint32_t			msg_query_interval, 
					const std::string&	pwd_machine_ip, 
					uint32_t			pwd_machine_port,
					uint32_t			pwd_machine_time_out,
					uint8_t				pin_pad_pwd_len,
					uint8_t				pin_pad_timeout);

	uint32_t MsgCountQueryInterval() const; // Return value in second.
	std::string PWDMachineIP() const; // Don't worry about the cost of returning string object because we have RVO & move semantics.
	uint32_t PWDMachinePort() const;
	uint32_t PWDMachineTimeout() const;
	uint8_t PINPadPWDLen() const; // The length of the password read from PIN pad.
	uint8_t PINPadTimeout() const; // Return value in seconds.
	std::string ClientCertStore() const;
	std::string ClientCertSubject() const;
private:
	ApplicationConfig();
	ApplicationConfig(const ApplicationConfig&);
	ApplicationConfig& operator=(const ApplicationConfig&);

	uint32_t	m_msg_count_query_interval;

	std::string m_pwd_machine_ip;
	uint32_t	m_pwd_machine_port;
	uint32_t	m_pwd_machine_timeout;

	uint8_t		m_pin_pad_pwd_len;
	uint8_t		m_pin_pad_timeout;

	std::string m_client_cert_store;
	std::string m_client_cert_subject;
};
#endif // ETRADECLIENT_UTILITY_APPLICATION_CONFIG_H_INCLUDED
