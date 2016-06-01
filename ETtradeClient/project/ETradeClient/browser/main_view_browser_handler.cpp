#include "stdafx.h"
#include "etradeclient/browser/main_view_browser_handler.h"

#include <stdio.h>
#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_path_util.h"
#include "include/cef_process_util.h"
#include "include/cef_runnable.h"
#include "include/cef_trace.h"
#include "include/base/cef_lock.h"
#include "include/wrapper/cef_helpers.h"

#include "etradeclient/browser/browser_util.h"
#include "etradeclient/browser/render_delegate.h"
#include "etradeclient/browser/session.h"
#include "etradeclient/browser/error_page.h"
#include "etradeclient/browser/async_js_callback_handler.h"
#include "etradeclient/utility/win_msg_define.h"
#include "etradeclient/utility/string_converter.h"

MainViewBrowserHandler::MainViewBrowserHandler()
{}

MainViewBrowserHandler::~MainViewBrowserHandler()
{}

//@{ CefClient methods
bool MainViewBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();
	if(m_browser_msg_router->OnProcessMessageReceived(browser, source_process, message)) 
		return true;
	return false;
}
//@}

//@{ CefLifeSpanHandler methods
void MainViewBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if(!m_browser_msg_router)
	{
		// Create the browser-side router for query handling.
		CefMessageRouterConfig config;
		m_browser_msg_router = CefMessageRouterBrowserSide::Create(config);

		//Create other message handlers here.
		AsyncJSCallbackHandler::Create(m_msg_handlers);

		// Register handlers with the router.
		for (const auto& handler : m_msg_handlers)
			m_browser_msg_router->AddHandler(handler, false);
	}

	auto is_popup = browser->IsPopup(); // @todo remove
	// get browser ID
	INT nBrowserId = browser->GetIdentifier();
	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());

	// Send msg to View to hold reference to this browser
	::SendMessage(hWindow, WM_CEF_NEW_BROWSER, (WPARAM)nBrowserId, (LPARAM)browser.get());

	// call parent
	CefLifeSpanHandler::OnAfterCreated(browser);
}

bool MainViewBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	HWND hwnd = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window
	::SendMessage(hwnd, WM_CEF_CLOSE_BROWSER, (WPARAM)browser.get(), (LPARAM)NULL); // send close browser notification.
	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void MainViewBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
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

bool MainViewBrowserHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									const CefString& target_url,
									const CefString& target_frame_name,
									cef_window_open_disposition_t target_disposition,
									bool user_gesture,
									const CefPopupFeatures& popupFeatures,
									CefWindowInfo& windowInfo,
									CefRefPtr<CefClient>& client,
									CefBrowserSettings& settings,
									bool* no_javascript_access)
{
	CEF_REQUIRE_IO_THREAD();

	if(browser->GetHost()->IsWindowRenderingDisabled())
		return true; // Cancel popups in off-screen rendering mode.

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent( browser->GetHost()->GetWindowHandle() );

	// send message
	LPCTSTR lpszURL(target_url.c_str());
	if( ::SendMessage( hWindow, WM_CEF_WINDOW_CHECK, (WPARAM)&popupFeatures, (LPARAM)lpszURL) == S_FALSE )
		return true;

	// send message
	if( ::SendMessage( hWindow, WM_CEF_NEW_WINDOW, (WPARAM)&popupFeatures, (LPARAM)&windowInfo) == S_FALSE )
		return true;
	
	// call parent, The |client| and |settings| values will default to the source browser's values.
	return CefLifeSpanHandler::OnBeforePopup(browser, frame, target_url, target_frame_name, target_disposition, user_gesture, popupFeatures, windowInfo, client, settings, no_javascript_access);
}
// @}

//@{ CefContextMenuHandler methods
void MainViewBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
#ifndef _DEBUG
	// Block the context menu for safety consideration. !!!BUT we should still allow context menu for "selection" & "editable" items.
	if ((params->GetTypeFlags() & (CM_TYPEFLAG_SELECTION | CM_TYPEFLAG_EDITABLE)) == 0)
	{
		model->Clear();
		return;
	}
#endif
	// call parent to get the default behavior.
	CefContextMenuHandler::OnBeforeContextMenu(browser, frame, params, model);
}

bool MainViewBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	// call parent to get the default behavior.
	return CefContextMenuHandler::OnContextMenuCommand(browser, frame, params, command_id, event_flags);
}
//@}

//@{ CefLoadHandler methods
void MainViewBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool is_loading, bool can_go_back, bool can_go_forward)
{
	CEF_REQUIRE_UI_THREAD();

	INT state = 0;
	// set state
	if( is_loading )
		state |= CEF_BIT_IS_LOADING;
	if( can_go_back )
		state |= CEF_BIT_CAN_GO_BACK;
	if( can_go_forward )
		state |= CEF_BIT_CAN_GO_FORWARD;

	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window.
	::SendMessage(hWindow, WM_CEF_STATE_CHANGE, (WPARAM)state, NULL); // send message.

	CefLoadHandler::OnLoadingStateChange(browser, is_loading, can_go_back, can_go_forward); // call parent.
}
//@}

//@{ CefDisplayHandler methods
void MainViewBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	CEF_REQUIRE_UI_THREAD();

	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window.
	LPCTSTR pszURL(url.c_str());
	::SendMessage( hWindow, WM_CEF_ADDRESS_CHANGE, (WPARAM)pszURL, NULL );
	
	CefDisplayHandler::OnAddressChange(browser, frame, url); // call parent.
}

void MainViewBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window.
	LPCTSTR pszTitle(title.c_str());
	::SendMessage( hWindow, WM_CEF_TITLE_CHANGE, (WPARAM)pszTitle, NULL );

	CefDisplayHandler::OnTitleChange(browser, title); // call parent.
}

void MainViewBrowserHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
	CEF_REQUIRE_UI_THREAD();
#if _DEBUG
	SendStatusToMainWnd(browser, value);
#endif // _DEBUG
	CefDisplayHandler::OnStatusMessage(browser, value); // call parent.
}

bool MainViewBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
	CEF_REQUIRE_UI_THREAD();
	return TRUE;
}
//@}

//@{ CefDownloadHandler methods
void MainViewBrowserHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
	CEF_REQUIRE_UI_THREAD();

	// Continue the download and show the "Save As" dialog.
	callback->Continue(GetDownloadPath(suggested_name), true);
}

void MainViewBrowserHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback)
{
	CEF_REQUIRE_UI_THREAD();

	CEFDownloadItemValues values;

	values.bIsValid = download_item->IsValid();
	values.bIsInProgress = download_item->IsInProgress();
	values.bIsComplete = download_item->IsComplete();
	values.bIsCanceled = download_item->IsCanceled();
	values.nProgress = download_item->GetPercentComplete();
	values.nSpeed = download_item->GetCurrentSpeed();
	values.nReceived = download_item->GetReceivedBytes();
	values.nTotal = download_item->GetTotalBytes();

	CefString& szDispo = download_item->GetContentDisposition();

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent( browser->GetHost()->GetWindowHandle() );

	// send message
	::SendMessage( hWindow, WM_CEF_DOWNLOAD_UPDATE, (WPARAM)&values, NULL );
}
//@}

//@{ CefLoadHandler methods
void MainViewBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent( browser->GetHost()->GetWindowHandle() );
	::SendMessage(hWindow, WM_CEF_LOAD_START, NULL, NULL);
	SendStatusToMainWnd(browser, L"页面加载中...");

	CefLoadHandler::OnLoadStart(browser, frame); // call parent
}

void MainViewBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code)
{
	CEF_REQUIRE_UI_THREAD();
	static const int kHttpCode_OK = 200; // @TODO 统一定义这些值
	static const int kFileCode_OK = 0; // code of loading local file

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent( browser->GetHost()->GetWindowHandle() );
	CefString url = frame->GetURL();
	LPCTSTR url_(url.c_str()); // WARNNING:LPCTSTR url_( frame->GetURL().c_str()) will get an incorrect string, don't do this way!
	::SendMessage(hWindow, WM_CEF_LOAD_END, http_status_code, (LPARAM)url_);

	if (kHttpCode_OK == http_status_code || kFileCode_OK == http_status_code)
		SendStatusToMainWnd(browser, L"页面加载完成!");
	else
		SendStatusToMainWnd(browser, L"页面加载出错!");
	
	CefLoadHandler::OnLoadEnd(browser, frame, http_status_code); // call parent
}

void MainViewBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode error_code, const CefString& error_text, const CefString& failed_url)
{
	CEF_REQUIRE_UI_THREAD();

	SendStatusToMainWnd(browser, L"页面加载结束!");

	// Don't display an error for if request aborted.
	if (error_code == ERR_ABORTED)
		return;

	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());
	LPCTSTR failed_url_(failed_url.c_str());
	CefString error_code_str = ErrorPage::ToErrorString(error_code);
	::SendMessage(hWindow, WM_CEF_LOAD_ERROR, (WPARAM)failed_url_, (LPARAM)error_code_str.c_str());

	// Don't display an error for external protocols that we allow the OS to handle. See OnProtocolExecution().
	if (error_code == ERR_UNKNOWN_URL_SCHEME)
	{
		std::string urlStr = frame->GetURL();
		if (urlStr.find("spotify:") == 0)
			return;
	}
	frame->LoadURL(ErrorPage::Url(failed_url, error_code, error_text)); // Load the error page.
}
//@}


//@{ CefRequestHandler methods
CefRefPtr<CefResourceHandler> MainViewBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
	CEF_REQUIRE_IO_THREAD();
	return NULL;
}

bool MainViewBrowserHandler::GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback)
{
	// The frame window will be the parent of the browser window
	HWND hWindow = GetParent( browser->GetHost()->GetWindowHandle() );

	CEFAuthenticationValues values;
	values.lpszHost = host.c_str();
	values.lpszRealm = realm.c_str();

	// send info
	if(::SendMessage( hWindow, WM_CEF_AUTHENTICATE, (WPARAM)&values, (LPARAM)NULL ) == S_OK)
	{
		callback->Continue( values.szUserName, values.szUserPass );
		return TRUE;
	}
	// canceled
	return FALSE;
}

bool MainViewBrowserHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefRequestCallback> callback)
{
	static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

	// Grant the quota request if the size is reasonable.
	callback->Continue(new_size <= max_size);

	// call parent
	return CefRequestHandler::OnQuotaRequest(browser, origin_url, new_size, callback);
}

void MainViewBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution)
{
	// do default
	CefRequestHandler::OnProtocolExecution(browser, url, allow_os_execution);
}

bool MainViewBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();
	if (Session::Instance().IsExpired()) // Return true to cancel the navigation if the session is already expired.
		return true;

	m_browser_msg_router->OnBeforeBrowse(browser, frame);

	CefString url = request->GetURL(); // get URL requested
	LPCTSTR url_(url.c_str()); // WARNNING:LPCTSTR url_( frame->GetURL().c_str()) will get an incorrect string, don't do this way!
	HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle()); // The frame window will be the parent of the browser window。
	if (::SendMessage(hWindow, WM_CEF_BEFORE_BROWSE, (WPARAM)url_, (LPARAM)is_redirect) == S_FALSE)
	{
		// cancel navigation
		return true;
	}
	SendStatusToMainWnd(browser, L"开始加载页面...");
	return CefRequestHandler::OnBeforeBrowse(browser, frame, request, is_redirect); // call parent
}

bool MainViewBrowserHandler::OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback)
{
	std::wstringstream text;

	// no file, or empty, show the default
	text << "The site's security certificate is not trusted!\n\n";
	text << "You attempted to reach: " << request_url.c_str() << " \n";
	text << "But the server presented a certificate issued by an entity that is not trusted by your computer's operating system.";
	text << "This may mean that the server has generated its own security credentials, ";
	text << "which Chrome cannot rely on for identity information, or an attacker may be ";
	text << "trying to intercept your communications.\n\n";
	text << "You should not proceed, especially if you have never seen this warning before for this site.";

	if (MessageBox(NULL, text.str().c_str(), L"The site's security certificate is not trusted:", MB_YESNO) == IDNO)
		return FALSE;

	// continue
	callback->Continue(true);

	return TRUE;
}

CefRequestHandler::ReturnValue MainViewBrowserHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
	// do defulat
	return CefRequestHandler::OnBeforeResourceLoad(browser, frame, request, callback);
}

bool MainViewBrowserHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response)
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
			// AfxGetMainWnd() will return NULL if called from an other thread (has to do with thread local storage).
			// That's why we use AfxGetApp()->GetMainWnd() instead of AfxGetMainWnd().
			PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CEF_SESSION_EXPIRED, NULL, NULL); // Post message to CMainFrame to try re-login.
		}
		
		if (RT_XHR != request->GetResourceType()) // RT_XHR means "XMLHttpRequest", don't redirect request if it is "XMLHttpRequest".
		{
			request->Set("about:blank", "GET", nullptr, CefRequest::HeaderMap()); // Redirect the URL to trigger another "OnBeforeBrowse".
			return true; // Return true to redirect.
		}
	}
	return false;
}

void MainViewBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
	CEF_REQUIRE_UI_THREAD();

	m_browser_msg_router->OnRenderProcessTerminated(browser);

	return CefRequestHandler::OnRenderProcessTerminated(browser, status);
}
void MainViewBrowserHandler::OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& plugin_path)
{
	return CefRequestHandler::OnPluginCrashed(browser, plugin_path);
}
//@}

//@{ CefJSDialogHandler methods
bool MainViewBrowserHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text, bool is_reload, CefRefPtr<CefJSDialogCallback> callback)
{
	// do defulat
	return FALSE;
}

void MainViewBrowserHandler::OnDialogClosed(CefRefPtr<CefBrowser> browser)
{
}

bool MainViewBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, CefJSDialogHandler::JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message)
{
	// do default
	suppress_message = FALSE;
	return FALSE;
}

void MainViewBrowserHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
}

//@}

std::string MainViewBrowserHandler::GetDownloadPath(const std::string& file_name)
{
	TCHAR szFolderPath[MAX_PATH];
	std::string path;

	// Save the file in the user's "My Documents" folder.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
		NULL, 0, szFolderPath))) {
			path = CefString(szFolderPath);
			path += "\\" + file_name;
	}
	return path;
}

void MainViewBrowserHandler::SendStatusToMainWnd(CefRefPtr<CefBrowser> browser, const std::wstring& status)
{
	if (!status.empty())
	{
		// The frame window will be the parent of the browser window
		HWND hWindow = GetParent(browser->GetHost()->GetWindowHandle());
		LPCTSTR status_(status.c_str());
		::SendMessage(hWindow, WM_CEF_STATUS_MESSAGE, (WPARAM)status_, NULL);
	}
}
