#ifndef ETRADECLIENT_BROWSER_MAIN_VIEW_BROWSER_HANDLER_H_INCLUDED
#define ETRADECLIENT_BROWSER_MAIN_VIEW_BROWSER_HANDLER_H_INCLUDED

#include <list>
#include <map>
#include <set>
#include <string>

#include "include/base/cef_lock.h"
#include "include/wrapper/cef_message_router.h"
#include "include/cef_client.h"


/*
MainViewBrowserHandler is responsible for handling main view's browser-level callbacks.
These callbacks are executed in the browser process.
*/


class MainViewBrowserHandler : 
	public CefClient, 
	public CefContextMenuHandler, 
	public CefDisplayHandler, 
	public CefDownloadHandler, 
	public CefLifeSpanHandler, 
	public CefLoadHandler, 
	public CefRequestHandler, 
	public CefJSDialogHandler
{
public:
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

	MainViewBrowserHandler();
	~MainViewBrowserHandler();

	//@{ CefClient methods
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE	{ return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE			{ return this; }
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE			{ return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE			{ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE					{ return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE			{ return this; }
	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE			{ return this; }

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
	//@}

	//@{ CefLifeSpanHandler methods
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnBeforePopup(	CefRefPtr<CefBrowser> browser, 
								CefRefPtr<CefFrame> frame, 
								const CefString& target_url, 
								const CefString& target_frame_name, 
								cef_window_open_disposition_t target_disposition, 
								bool user_gesture, 
								const CefPopupFeatures& popupFeatures, 
								CefWindowInfo& windowInfo, 
								CefRefPtr<CefClient>& client, 
								CefBrowserSettings& settings, 
								bool* no_javascript_access) OVERRIDE;
	//@}

	//@{ CefContextMenuHandler methods
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, 
									 CefRefPtr<CefFrame> frame, 
									 CefRefPtr<CefContextMenuParams> params, 
									 CefRefPtr<CefMenuModel> model) OVERRIDE;
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, 
									  CefRefPtr<CefFrame> frame, 
									  CefRefPtr<CefContextMenuParams> params, 
									  int command_id, 
									  EventFlags event_flags) OVERRIDE;
	//@}
	
	//@{ CefLoadHandler methods
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool is_loading, bool can_go_back, bool can_go_forward) OVERRIDE;
	//@}

	//@{ CefDisplayHandler methods
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
	virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
	virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;
	//@}

	//@{ CefDownloadHandler methods
	virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, 
								  CefRefPtr<CefDownloadItem> download_item, 
								  const CefString& suggested_name, 
								  CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
	virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, 
								   CefRefPtr<CefDownloadItem> download_item, 
								   CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;
	//@}

	//@{ CefLoadHandler methods
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code) OVERRIDE;
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, 
							 CefRefPtr<CefFrame> frame, 
							 ErrorCode errorCode, 
							 const CefString& errorText, 
							 const CefString& failedUrl) OVERRIDE;
	//@}

	//@{ CefRequestHandler methods
	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, 
															 CefRefPtr<CefFrame> frame, 
															 CefRefPtr<CefRequest> request) OVERRIDE;
	virtual bool GetAuthCredentials(CefRefPtr<CefBrowser> browser, 
									CefRefPtr<CefFrame> frame, 
									bool isProxy, 
									const CefString& host, 
									int port, 
									const CefString& realm, 
									const CefString& scheme, 
									CefRefPtr<CefAuthCallback> callback) OVERRIDE;
	virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser, 
								const CefString& origin_url, 
								int64 new_size, 
								CefRefPtr<CefRequestCallback> callback) OVERRIDE;
	virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser, 
									 const CefString& url, 
									 bool& allow_os_execution) OVERRIDE;
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, 
								CefRefPtr<CefFrame> frame, 
								CefRefPtr<CefRequest> request, 
								bool is_redirect) OVERRIDE;
	virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser,
									cef_errorcode_t cert_error,
									const CefString& request_url,
									CefRefPtr<CefSSLInfo> ssl_info,
									CefRefPtr<CefRequestCallback> callback) OVERRIDE;
	virtual ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
											  CefRefPtr<CefFrame> frame, 
											  CefRefPtr<CefRequest> request,
											  CefRefPtr<CefRequestCallback> callback) OVERRIDE;

	virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request,
									CefRefPtr<CefResponse> response) OVERRIDE;

	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) OVERRIDE;

	virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser, const CefString& plugin_path) OVERRIDE;
	//@}

	//@{ CefJSDialogHandler methods
	virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, 
									  const CefString& message_text, 
									  bool is_reload, 
									  CefRefPtr<CefJSDialogCallback> callback) OVERRIDE;
	virtual void OnDialogClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser, 
							const CefString& origin_url, 
							const CefString& accept_lang, 
							CefJSDialogHandler::JSDialogType dialog_type, 
							const CefString& message_text, 
							const CefString& default_prompt_text, 
							CefRefPtr<CefJSDialogCallback> callback, 
							bool& suppress_message) OVERRIDE;
	virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) OVERRIDE;
	//@}

private:
	// Returns the full download path for the specified file, or an empty path to use the default temp directory.
	std::string GetDownloadPath(const std::string& file_name);
	void SendStatusToMainWnd(CefRefPtr<CefBrowser> browser, const std::wstring& status);
	void LoadErrorPage(CefRefPtr<CefFrame> frame, const std::string& failed_url, cef_errorcode_t error_code, const std::string& other_info);
	std::string GetErrorString(cef_errorcode_t code);
private:
	CefRefPtr<CefMessageRouterBrowserSide>	m_browser_msg_router; // Handles the browser side of query routing.
	MessageHandlerSet						m_msg_handlers; // Hold all the browser side message handers.

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(MainViewBrowserHandler);
};

#endif // ETRADECLIENT_BROWSER_MAIN_VIEW_BROWSER_HANDLER_H_INCLUDED
