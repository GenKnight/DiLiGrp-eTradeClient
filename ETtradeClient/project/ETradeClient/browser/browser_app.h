#ifndef ETRADECLIENT_BROWSER_BROWSER_APP_H_INCLUDED
#define ETRADECLIENT_BROWSER_BROWSER_APP_H_INCLUDED

#include "include/cef_app.h"
#include "etradeclient/browser/render_delegate.h"

/*
BrowserApp is responsible for handling process-level callbacks. 
It exposes some interfaces/methods that are shared by multiple processes 
and some that are only called in a particular process.
*/
class BrowserApp: public CefApp, // The CefApp interface provides access to process-specific callbacks.
				  public CefBrowserProcessHandler,
				  public CefRenderProcessHandler
{
	typedef RenderDelegate::BrowserRenderSet BrowserRenderDelegateSetT;

public:
	BrowserApp();
	~BrowserApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

	// CefRenderProcessHandler methods.
	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	virtual void OnWebKitInitialized() OVERRIDE;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavigationType navigation_type, bool is_redirect) OVERRIDE;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace)OVERRIDE;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

	// Creates all of the RenderDelegate objects.
	void CreateRenderDelegates(RenderDelegate::BrowserRenderSet& delegates);

private:
	BrowserRenderDelegateSetT	m_browser_render_delegates; // Set of supported RenderDelegates. Only used in the renderer process.
	
	IMPLEMENT_REFCOUNTING(BrowserApp); // Include the default reference counting implementation.
};

#endif // ETRADECLIENT_BROWSER_BROWSER_APP_H_INCLUDED
