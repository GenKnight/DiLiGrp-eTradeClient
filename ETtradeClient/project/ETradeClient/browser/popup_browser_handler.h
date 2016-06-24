#ifndef ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED
#define ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED

#include <list>
#include <map>
#include <set>
#include <string>

#include "include/base/cef_lock.h"
#include "include/wrapper/cef_message_router.h"
#include "include/cef_client.h"

/*
PopupBrowserHandler is responsible for handling popup browser callbacks.
These callbacks are executed in the browser process.
*/

class PopupBrowserHandler :
	public CefClient,
	public CefContextMenuHandler,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefRequestHandler,
	public CefLoadHandler,
	public CefJSDialogHandler
{
public:
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

	PopupBrowserHandler(void){};
	~PopupBrowserHandler(void){};


	//@{ CefClient methods
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE{ return this; }

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
	//@}

	//@{ CefDisplayHandler methods
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
	//@}
	
	//@{ CefLifeSpanHandler methods
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	//@}
	
	//@{ CefRequestHandler methods
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) OVERRIDE;
	virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response) OVERRIDE;
	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) OVERRIDE;
	//@}

	//@{ CefLoadHandler methods
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code) OVERRIDE;
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode error_code, const CefString& error_text, const CefString& failed_url) OVERRIDE;
	//@}

	//@{ CefContextMenuHandler methods
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;
	//@}

	//@{ CefJSDialogHandler methods
	virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url,	const CefString& accept_lang, CefJSDialogHandler::JSDialogType dialog_type, 
		const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message) OVERRIDE;
	//@}
private:
	CefRefPtr<CefMessageRouterBrowserSide>	m_browser_msg_router; // Handles the browser side of query routing.
	MessageHandlerSet						m_msg_handlers; // Hold all the browser side message handers.

private:
	IMPLEMENT_REFCOUNTING(PopupBrowserHandler);
	DISALLOW_COPY_AND_ASSIGN(PopupBrowserHandler);
};

#endif // ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED
