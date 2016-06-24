#ifndef ETRADECLIENT_BROWSER_SESSION_H_INCLUDED
#define ETRADECLIENT_BROWSER_SESSION_H_INCLUDED

#include <mutex>

#include "etradeclient/utility/win_http.h"

/*A "singleton class to provide a universal interface to access."*/
class Session
{
public:
	typedef WinHttp::Cookies CookiesT;

	static Session& Instance();
	void OnExpired(); // Triggered when session is expired, the session will be in "expired" state after being called.
	void OnValid(); // Validate the session, the session will be in "not expired" state after being called.
	bool IsExpired();
	bool IsFirstLogin();
	void SetFirstLoginFlag(bool is_first);
	void SetCookies(const CookiesT& cookies);
	const CookiesT& Cookies() const;
	void SetUserName(const std::wstring& user_name);
	const std::wstring& UserName() const;
private:
	Session();
	
	bool			m_expried;
	bool			m_is_first_login;
	std::mutex		m_mtx;
	CookiesT		m_cookies;
	std::wstring	m_user_name;
};
#endif // ETRADECLIENT_BROWSER_SESSION_H_INCLUDED
