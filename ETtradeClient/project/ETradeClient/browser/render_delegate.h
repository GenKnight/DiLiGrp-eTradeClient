#ifndef ETRADECLIENT_BROWSER_RENDER_DELEGATE_H_INCLUDED
#define ETRADECLIENT_BROWSER_RENDER_DELEGATE_H_INCLUDED

#include <set>

#include "include/cef_base.h"
#include "include/cef_process_message.h"

// Forward declaration
class BrowserApp; 
class CefListValue;
class CefBrowser;
class CefFrame;
class CefRequest;
class CefV8Context;
class CefV8Exception;
class CefV8StackTrace;
class CefDOMNode;
class CefProcessMessage;
class CefLoadHandler;


namespace RenderDelegate
{
// Interface for renderer delegates. All RenderDelegates must be returned via
	// CreateRenderDelegates. Do not perform work in the RenderDelegate
	// constructor. See CefRenderProcessHandler for documentation.
	class BrowserRender : public virtual CefBase {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<BrowserApp> app, CefRefPtr<CefListValue> extra_info);
		virtual void OnWebKitInitialized(CefRefPtr<BrowserApp> app);
		virtual void OnBrowserCreated(CefRefPtr<BrowserApp> app, CefRefPtr<CefBrowser> browser);
		virtual void OnBrowserDestroyed(CefRefPtr<BrowserApp> app, CefRefPtr<CefBrowser> browser);
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<BrowserApp> app);
		virtual bool OnBeforeNavigation(CefRefPtr<BrowserApp> app,
										CefRefPtr<CefBrowser> browser,
										CefRefPtr<CefFrame> frame,
										CefRefPtr<CefRequest> request,
										cef_navigation_type_t navigation_type,
										bool is_redirect);
		virtual void OnContextCreated(CefRefPtr<BrowserApp> app,
									  CefRefPtr<CefBrowser> browser, 
									  CefRefPtr<CefFrame> frame, 
									  CefRefPtr<CefV8Context> context);
		virtual void OnContextReleased(CefRefPtr<BrowserApp> app,
									   CefRefPtr<CefBrowser> browser, 
									   CefRefPtr<CefFrame> frame, 
									   CefRefPtr<CefV8Context> context);
		virtual void OnUncaughtException(CefRefPtr<BrowserApp> app,
										 CefRefPtr<CefBrowser> browser, 
										 CefRefPtr<CefFrame> frame, 
										 CefRefPtr<CefV8Context> context, 
										 CefRefPtr<CefV8Exception> exception, 
										 CefRefPtr<CefV8StackTrace> stackTrace);

		virtual bool OnProcessMessageReceived(CefRefPtr<BrowserApp> app, 
											  CefRefPtr<CefBrowser> browser, 
											  CefProcessId source_process, 
											  CefRefPtr<CefProcessMessage> message);
	};


	typedef std::set<CefRefPtr<BrowserRender>> BrowserRenderSet;

	// Create the render delegate.
	void CreateRenderDelegates(BrowserRenderSet& delegates);

} // RenderDelegate

#endif  // ETRADECLIENT_BROWSER_RENDER_DELEGATE_H_INCLUDED
