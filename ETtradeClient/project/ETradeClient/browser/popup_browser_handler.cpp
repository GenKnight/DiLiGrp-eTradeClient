#include "stdafx.h"
#include "etradeclient/browser/popup_browser_handler.h"

#include "include/wrapper/cef_helpers.h"

#include "etradeclient/utility/session.h"
#include "etradeclient/utility/win_msg_define.h"
#include "etradeclient/browser/error_page.h"
#include "etradeclient/browser/async_js_callback_handler.h"


bool PopupBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();
	if (m_browser_msg_router->OnProcessMessageReceived(browser, source_process, message))
		return true;
	return false;
}

void PopupBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window.
	LPCTSTR pszTitle(title.c_str());
	::SendMessage(hWindow, WM_CEF_TITLE_CHANGE, (WPARAM)pszTitle, NULL);

	CefDisplayHandler::OnTitleChange(browser, title); // call parent.
}

void PopupBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();


	if (!m_browser_msg_router)
	{
		// Create the browser-side router for query handling.
		CefMessageRouterConfig config;
		m_browser_msg_router = CefMessageRouterBrowserSide::Create(config);

		//Create other message handlers here.
		AsyncJSCallbackHandler::UI_Create(m_msg_handlers);

		// Register handlers with the router.
		for (const auto& handler : m_msg_handlers)
			m_browser_msg_router->AddHandler(handler, false);
	}


	::SendMessage(GetParent(browser->GetHost()->GetWindowHandle()), 
		WM_CEF_MSG_VIEW_NEW_BROWSER, (WPARAM)browser.get(), NULL);
	CefLifeSpanHandler::OnAfterCreated(browser);
}

bool PopupBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	::SendMessage(GetParent(browser->GetHost()->GetWindowHandle()), 
		WM_CEF_MSG_VIEW_CLOSE_BROWSER, (WPARAM)browser.get(), NULL);
	return CefLifeSpanHandler::DoClose(browser);
}

void PopupBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	m_browser_msg_router->OnBeforeClose(browser);

	// All browser windows have closed.
	// Remove and delete message router handlers.
	for (const auto& handler : m_msg_handlers)
		m_browser_msg_router->RemoveHandler(handler);

	m_msg_handlers.clear();
	m_browser_msg_router = NULL;

	CefLifeSpanHandler::OnBeforeClose(browser); // call parent
}


bool PopupBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();
	if (Session::Instance().IsExpired()) // Return true to cancel the navigation if the session is already expired.
		return true;

	m_browser_msg_router->OnBeforeBrowse(browser, frame);

	return CefRequestHandler::OnBeforeBrowse(browser, frame, request, is_redirect);
}

bool PopupBrowserHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response)
{
	CEF_REQUIRE_IO_THREAD();

	const CefString kTimeoutHeader("sessionStatus");
			const CefString kTimeout("expired");

	auto& url = request->GetURL();
	CefRequest::HeaderMap headerMap;
	response->GetHeaderMap(headerMap);

	if (0 == response->GetHeader(kTimeoutHeader).compare(kTimeout)) // If timeout.
	{
		if (!Session::Instance().IsExpired())// Avoid repeating post message to handle the session expired.
		{
			Session::Instance().OnExpired();
			::PostMessage(GetParent(browser->GetHost()->GetWindowHandle()), WM_CEF_SESSION_EXPIRED, NULL, NULL);
		}

		if (RT_XHR != request->GetResourceType()) // RT_XHR means "XMLHttpRequest", don't redirect request if it is "XMLHttpRequest".
		{
			request->Set("about:blank", "GET", nullptr, CefRequest::HeaderMap()); // Redirect the URL to trigger another "OnBeforeBrowse".
			return true; // Return true to redirect.
		}
	}
	return false;
}

void PopupBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
	CEF_REQUIRE_UI_THREAD();

	m_browser_msg_router->OnRenderProcessTerminated(browser);

	return CefRequestHandler::OnRenderProcessTerminated(browser, status);
}

void PopupBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code)
{
	CEF_REQUIRE_UI_THREAD();

	// The frame window will be the parent of the browser window
	::SendMessage(GetParent(browser->GetHost()->GetWindowHandle()), WM_CEF_LOAD_END, NULL, NULL);
	// call parent
	CefLoadHandler::OnLoadEnd(browser, frame, http_status_code);
}

void PopupBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode error_code, const CefString& error_text, const CefString& failed_url)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (error_code == ERR_ABORTED)
		return;

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());
	LPCTSTR failed_url_(failed_url.c_str());
	CefString error_code_str = ErrorPage::ToErrorString(error_code);
	::SendMessage(hWindow, WM_CEF_LOAD_ERROR, (WPARAM)failed_url_, (LPARAM)error_code_str.c_str());

	// Don't display an error for external protocols that we allow the OS to handle. See OnProtocolExecution().
	if (error_code == ERR_UNKNOWN_URL_SCHEME) {
		std::string urlStr = frame->GetURL();
		if (urlStr.find("spotify:") == 0)
			return;
	}
	frame->LoadURL(ErrorPage::Url(failed_url, error_code, error_text)); // Load the error page.
}

//@{ CefContextMenuHandler methods
void PopupBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	// Block the context menu for safety consideration. !!!BUT we should still allow context menu for "selection" & "editable" items.
	if ((params->GetTypeFlags() & (CM_TYPEFLAG_SELECTION | CM_TYPEFLAG_EDITABLE)) == 0)
	{
		model->Clear();
		return;
	}
	// call parent to get the default behavior.
	CefContextMenuHandler::OnBeforeContextMenu(browser, frame, params, model);
}

bool PopupBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	// call parent to get the default behavior.
	return CefContextMenuHandler::OnContextMenuCommand(browser, frame, params, command_id, event_flags);
}
//@}

//@{ CefJSDialogHandler methods
bool PopupBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, CefJSDialogHandler::JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message)
{
	// do default
	suppress_message = FALSE;
	return FALSE;
}
//@}