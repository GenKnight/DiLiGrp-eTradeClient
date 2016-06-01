#ifndef ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED
#define ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED

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
	public CefLoadHandler
{
public:
	PopupBrowserHandler(void){};
	~PopupBrowserHandler(void){};

	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{ return this; }
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; }
	CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{ return this; }
	CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{ return this; }

	//@{ CefDisplayHandler methods
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
	//@}

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) OVERRIDE;
	virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code) OVERRIDE;
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode error_code, const CefString& error_text, const CefString& failed_url) OVERRIDE;
	//@{ CefContextMenuHandler methods
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;
	//@}
private:
	IMPLEMENT_REFCOUNTING(PopupBrowserHandler);
	DISALLOW_COPY_AND_ASSIGN(PopupBrowserHandler);
};

#endif // ETRADECLIENT_BROWSER_POPUP_BROWSER_HANDLER_H_INCLUDED
