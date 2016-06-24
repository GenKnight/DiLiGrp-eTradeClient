#include "stdafx.h"

#include "etradeclient/utility/session.h"

#include "include/wrapper/cef_helpers.h"

Session::Session() : m_expried(true), m_is_first_login(false) /*The initial state should be 'expired'.*/
{}

Session& Session::Instance()
{
	static Session session_manager;
	return session_manager;
}

void Session::OnExpired()
{
	std::lock_guard<std::mutex> lg(m_mtx); 
//	CEF_REQUIRE_IO_THREAD();

	m_cookies.clear();
	m_expried = true;
}

void Session::OnValid()
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_expried = false;
}

bool Session::IsExpired()
{
	std::lock_guard<std::mutex> lg(m_mtx);
	return m_expried;
}

void Session::SetCookies(const CookiesT& cookies)
{
	m_cookies = cookies;
}

const Session::CookiesT& Session::Cookies() const
{
	return m_cookies;
}

void Session::SetUserName(const std::wstring& user_name)
{
	m_user_name = user_name;
}

const std::wstring& Session::UserName() const
{
	return m_user_name;
}

bool Session::IsFirstLogin()
{
	return m_is_first_login;
}

void Session::SetFirstLoginFlag(bool is_first)
{
	m_is_first_login = is_first;
}

