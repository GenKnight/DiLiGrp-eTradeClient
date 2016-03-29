#include "stdafx.h"

#include "etradeclient/browser/browser_app.h"

#include <algorithm>

#include "include/wrapper/cef_helpers.h"

BrowserApp::BrowserApp()
{}

BrowserApp::~BrowserApp()
{}

CefRefPtr<CefBrowserProcessHandler> BrowserApp::GetBrowserProcessHandler()
{
	return this;
}
CefRefPtr<CefRenderProcessHandler> BrowserApp::GetRenderProcessHandler()
{
	return this;
}

// Called on the browser process UI thread immediately after the CEF context has been initialized.
void BrowserApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD()
}

void BrowserApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
	RenderDelegate::CreateRenderDelegates(m_browser_render_delegates);
	for (const auto& render : m_browser_render_delegates)
		render->OnRenderThreadCreated(this, extra_info);
}

void BrowserApp::OnWebKitInitialized()
{
	for (const auto& render : m_browser_render_delegates)
		render->OnWebKitInitialized(this);
}

void BrowserApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	for (const auto& render : m_browser_render_delegates)
		render->OnBrowserCreated(this, browser);
}

void BrowserApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
	for (const auto& render : m_browser_render_delegates)
		render->OnBrowserDestroyed(this, browser);
}

bool BrowserApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request,
									NavigationType navigation_type,
									bool is_redirect) 
{
	for (const auto& render : m_browser_render_delegates)
	{
		if (render->OnBeforeNavigation(this, browser, frame, request, navigation_type, is_redirect))
			return true;
	}
	return false;
}

void BrowserApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	for (const auto& render : m_browser_render_delegates)
		render->OnContextCreated(this, browser, frame, context);
}

void BrowserApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	for (const auto& render : m_browser_render_delegates)
		render->OnContextReleased(this, browser, frame, context);
}

void BrowserApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
									 CefRefPtr<CefFrame> frame, 
									 CefRefPtr<CefV8Context> context, 
									 CefRefPtr<CefV8Exception> exception, 
									 CefRefPtr<CefV8StackTrace> stackTrace)
{
	for (const auto& render : m_browser_render_delegates)
		render->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
}

bool BrowserApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	DCHECK_EQ(source_process, PID_BROWSER);

	bool handled = false;

	// TODO: Study the mechanism of OnProcessMessageReceived and refactor this code.
	RenderDelegate::BrowserRenderSet::iterator it = m_browser_render_delegates.begin();
	for (; it != m_browser_render_delegates.end() && !handled; ++it) 
	{
		handled = (*it)->OnProcessMessageReceived(this, browser, source_process, message);
	}

	return handled;
}

void BrowserApp::CreateRenderDelegates(BrowserRenderDelegateSetT& delegates)
{
	RenderDelegate::CreateRenderDelegates(delegates);
}
