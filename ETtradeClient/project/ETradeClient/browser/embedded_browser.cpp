#include "stdafx.h"

#include "etradeclient/browser/embedded_browser.h"

#include <mutex>
#include <condition_variable>

#include "include/cef_request.h"
#include "include/cef_cookie.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"

namespace
{
	class SetCookieProxy : public CefSetCookieCallback
	{
	public:
		SetCookieProxy(std::mutex& mtx, std::condition_variable& cv) : 
			m_mtx(mtx), m_cv(cv), m_set_cookie_done(false), m_set_cookie_res(false)
		{}

		void SetCookie(const std::string& url, const CefCookie& cookie)
		{
			CEF_REQUIRE_IO_THREAD();
			CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
			if (!manager->SetCookie(url, cookie, CefRefPtr<CefSetCookieCallback>(this)))
			{
				m_set_cookie_res = false; // Set cookie failed.
				m_set_cookie_done = true;
				m_cv.notify_one(); // Notify the other thread when setting cookie failed.
			}
		}
		void OnComplete(bool res)
		{
			CEF_REQUIRE_IO_THREAD();
			{
				std::lock_guard<std::mutex> lg(m_mtx);
				m_set_cookie_res = res;
				m_set_cookie_done = true;
			}
			m_cv.notify_one();
		}
	private:
		std::mutex&					m_mtx;
		std::condition_variable&	m_cv;
		bool 						m_set_cookie_done;
		bool 						m_set_cookie_res;
	private:
		friend class EmbeddedBrower;

		IMPLEMENT_REFCOUNTING(SetCookieProxy);
		DISALLOW_COPY_AND_ASSIGN(SetCookieProxy);
	};
}


bool EmbeddedBrower::SetCookie(const std::string& url, const CefCookie& cookie)
{
	std::mutex mtx;
	std::condition_variable cv;
	CefRefPtr<SetCookieProxy> set_cookie_proxy = new SetCookieProxy(mtx, cv);
	CefPostTask(TID_IO, base::Bind(&SetCookieProxy::SetCookie, set_cookie_proxy, url, cookie));

	std::unique_lock<std::mutex> lk(mtx);
	cv.wait(lk, [&]
	{
		return set_cookie_proxy->m_set_cookie_done;
	});
	return set_cookie_proxy->m_set_cookie_res;
}


EmbeddedBrower::EmbeddedBrower() : m_cef_browser(nullptr)
{}
EmbeddedBrower::~EmbeddedBrower()
{
	if (m_cef_browser) // release browser if still active
		m_cef_browser = nullptr;
}

void EmbeddedBrower::AttachCEFBrowser(CefRefPtr<CefBrowser> cef_browser)
{
	m_cef_browser = cef_browser;
}
void EmbeddedBrower::DetachCEFBrowser(CefRefPtr<CefBrowser> cef_browser)
{
	if (nullptr != m_cef_browser && m_cef_browser->IsSame(cef_browser))
		m_cef_browser = nullptr; // Release reference of browser instance.
}
bool EmbeddedBrower::CEFBrowserAttached() const
{
	return (nullptr != m_cef_browser);
}

void EmbeddedBrower::Close(bool force_close)
{
	// Notify the browser window that we would like to close it. This
	// will result in a call to MainViewBrowserHandler::DoClose() if the
	// JavaScript 'onbeforeunload' event handler allows it.
	m_cef_browser->GetHost()->CloseBrowser(force_close);
}
void EmbeddedBrower::Resize(const RECT& rect)
{
	// cef stretch to to full view
	CefWindowHandle hwnd = m_cef_browser->GetHost()->GetWindowHandle();
	if (hwnd)
	{
		HDWP hdwp = BeginDeferWindowPos(1);
		hdwp = DeferWindowPos(hdwp, hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		EndDeferWindowPos(hdwp);
	}
}

void EmbeddedBrower::Copy()
{
	m_cef_browser->GetMainFrame()->Copy();
}
void EmbeddedBrower::Cut()
{
	m_cef_browser->GetMainFrame()->Cut();
}
void EmbeddedBrower::Paste()
{
	m_cef_browser->GetMainFrame()->Paste();
}

void EmbeddedBrower::NavigateTo(const std::string& url) const
{
	m_cef_browser->GetMainFrame()->LoadURL(url);
}
void EmbeddedBrower::LoadRequest(const CefRefPtr<CefRequest>& req) const
{
	m_cef_browser->GetMainFrame()->LoadRequest(req);
}

void EmbeddedBrower::GoBack() const
{
	m_cef_browser->GoBack();
}

void EmbeddedBrower::GoForward() const
{
	m_cef_browser->GoForward();
}

void EmbeddedBrower::Reload() const
{
	m_cef_browser->Reload();
}

void EmbeddedBrower::SetFocus(bool focus)
{
	m_cef_browser->GetHost()->SetFocus(true);
}