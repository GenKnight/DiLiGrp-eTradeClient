#ifndef ETRADECLIENT_UTILITY_LOGON_MGR_H_INCLUDED
#define ETRADECLIENT_UTILITY_LOGON_MGR_H_INCLUDED

#include <string>

class LogonMgr
{
public:
	static LogonMgr& Instance();

	bool DoLogin(const std::wstring& account, const std::wstring& pwd, const std::string& type);
	bool DoLogout();
	const std::wstring& ErrorMsg() const;
private:
	LogonMgr();
	LogonMgr(const LogonMgr&);
	LogonMgr& operator=(const LogonMgr&);

	bool GetLoginResult(const std::string& response_str); //Parse the response string which is a JSON string.
private:
	std::wstring m_err_msg;
};

#endif // ETRADECLIENT_UTILITY_LOGON_MGR_H_INCLUDED
