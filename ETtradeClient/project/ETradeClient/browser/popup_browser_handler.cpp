#include "stdafx.h"
#include "etradeclient/browser/popup_browser_handler.h"

#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"

#include "etradeclient/browser/session.h"
#include "etradeclient/utility/win_msg_define.h"

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

bool PopupBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();
	if (Session::Instance().IsExpired()) // Return true to cancel the navigation if the session is already expired.
		return true;
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
	CefString error_code_str = GetErrorString(error_code);
	::SendMessage(hWindow, WM_CEF_LOAD_ERROR, (WPARAM)failed_url_, (LPARAM)error_code_str.c_str());

	// Don't display an error for external protocols that we allow the OS to handle. See OnProtocolExecution().
	if (error_code == ERR_UNKNOWN_URL_SCHEME) {
		std::string urlStr = frame->GetURL();
		if (urlStr.find("spotify:") == 0)
			return;
	}
	LoadErrorPage(frame, failed_url, error_code, error_text); // Load the error page.
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

// Load a data: URI containing the error message.
void PopupBrowserHandler::LoadErrorPage(CefRefPtr<CefFrame> frame, const std::string& failed_url, cef_errorcode_t error_code, const std::string& other_info)
{
	std::stringstream ss;
	ss << "<html><head><title>ERROR!</title></head>"
		"<body bgcolor=\"white\">"
		"<h3>Page failed to load.</h3>"
		"<br/>Error: " << GetErrorString(error_code) <<
		" (" << error_code << ")";
	if (!other_info.empty())
		ss << "<br/>" << other_info;
	ss << "</body></html>";

	std::string html_data = ss.str();
	CefString data = CefBase64Encode(html_data.data(), html_data.size());
	std::wstring err_report_url = L"data:text/html;base64," +
		CefURIEncode(data, false).ToWString();
	frame->LoadURL(err_report_url);
}

std::string PopupBrowserHandler::GetErrorString(cef_errorcode_t code)
{
	// Case condition that returns |code| as a string.
#define CASE(code) case code: return #code

	switch (code) {
		CASE(ERR_NONE);
		CASE(ERR_FAILED);
		CASE(ERR_ABORTED);
		CASE(ERR_INVALID_ARGUMENT);
		CASE(ERR_INVALID_HANDLE);
		CASE(ERR_FILE_NOT_FOUND);
		CASE(ERR_TIMED_OUT);
		CASE(ERR_FILE_TOO_BIG);
		CASE(ERR_UNEXPECTED);
		CASE(ERR_ACCESS_DENIED);
		CASE(ERR_NOT_IMPLEMENTED);
		CASE(ERR_CONNECTION_CLOSED);
		CASE(ERR_CONNECTION_RESET);
		CASE(ERR_CONNECTION_REFUSED);
		CASE(ERR_CONNECTION_ABORTED);
		CASE(ERR_CONNECTION_FAILED);
		CASE(ERR_NAME_NOT_RESOLVED);
		CASE(ERR_INTERNET_DISCONNECTED);
		CASE(ERR_SSL_PROTOCOL_ERROR);
		CASE(ERR_ADDRESS_INVALID);
		CASE(ERR_ADDRESS_UNREACHABLE);
		CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
		CASE(ERR_TUNNEL_CONNECTION_FAILED);
		CASE(ERR_NO_SSL_VERSIONS_ENABLED);
		CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
		CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
		CASE(ERR_CERT_COMMON_NAME_INVALID);
		CASE(ERR_CERT_DATE_INVALID);
		CASE(ERR_CERT_AUTHORITY_INVALID);
		CASE(ERR_CERT_CONTAINS_ERRORS);
		CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
		CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
		CASE(ERR_CERT_REVOKED);
		CASE(ERR_CERT_INVALID);
		CASE(ERR_CERT_END);
		CASE(ERR_INVALID_URL);
		CASE(ERR_DISALLOWED_URL_SCHEME);
		CASE(ERR_UNKNOWN_URL_SCHEME);
		CASE(ERR_TOO_MANY_REDIRECTS);
		CASE(ERR_UNSAFE_REDIRECT);
		CASE(ERR_UNSAFE_PORT);
		CASE(ERR_INVALID_RESPONSE);
		CASE(ERR_INVALID_CHUNKED_ENCODING);
		CASE(ERR_METHOD_NOT_SUPPORTED);
		CASE(ERR_UNEXPECTED_PROXY_AUTH);
		CASE(ERR_EMPTY_RESPONSE);
		CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
		CASE(ERR_CACHE_MISS);
		CASE(ERR_INSECURE_RESPONSE);
	default:
		return "UNKNOWN";
	}
}