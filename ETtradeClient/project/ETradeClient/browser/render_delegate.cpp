#include "stdafx.h"

#include "etradeclient/browser/render_delegate.h"

#include "include/cef_values.h"
#include "include/cef_browser.h"
#include "include/cef_load_handler.h"

#include "etradeclient/browser/browser_app.h"
#include "etradeclient/browser/main_view_browser_handler.h"

namespace RenderDelegate
{
	void BrowserRender::OnRenderThreadCreated(CefRefPtr<BrowserApp> app, CefRefPtr<CefListValue> extra_info)
	{}
	void BrowserRender::OnWebKitInitialized(CefRefPtr<BrowserApp> app)
	{}
	void BrowserRender::OnBrowserCreated(CefRefPtr<BrowserApp> app, CefRefPtr<CefBrowser> browser)
	{}
	void BrowserRender::OnBrowserDestroyed(CefRefPtr<BrowserApp> app, CefRefPtr<CefBrowser> browser)
	{}
	CefRefPtr<CefLoadHandler> BrowserRender::GetLoadHandler(CefRefPtr<BrowserApp> app)
	{
		return NULL;
	}
	bool BrowserRender::OnBeforeNavigation(CefRefPtr<BrowserApp> app,
									CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request,
									cef_navigation_type_t navigation_type,
									bool is_redirect)
	{
		return false;
	}
	void BrowserRender::OnContextCreated(CefRefPtr<BrowserApp> app,
									CefRefPtr<CefBrowser> browser, 
									CefRefPtr<CefFrame> frame, 
									CefRefPtr<CefV8Context> context)
	{}
	void BrowserRender::OnContextReleased(CefRefPtr<BrowserApp> app,
									CefRefPtr<CefBrowser> browser, 
									CefRefPtr<CefFrame> frame, 
									CefRefPtr<CefV8Context> context)
	{}

	void BrowserRender::OnUncaughtException(CefRefPtr<BrowserApp> app,
										CefRefPtr<CefBrowser> browser, 
										CefRefPtr<CefFrame> frame, 
										CefRefPtr<CefV8Context> context, 
										CefRefPtr<CefV8Exception> exception, 
										CefRefPtr<CefV8StackTrace> stackTrace)
	{}

	// Called when a process message is received. Return true if the message was
	// handled and should not be passed on to other handlers. RenderDelegates
	// should check for unique message names to avoid interfering with each
	// other.
	bool BrowserRender::OnProcessMessageReceived(CefRefPtr<BrowserApp> app,
											CefRefPtr<CefBrowser> browser, 
											CefProcessId source_process, 
											CefRefPtr<CefProcessMessage> message)
	{
		return false;
	}

	class BrowserRenderDelegate : public BrowserRender
	{
	public:
		BrowserRenderDelegate()
		{}

		virtual void OnWebKitInitialized(CefRefPtr<BrowserApp> app) OVERRIDE
		{
			// Create the renderer-side router for query handling.
			CefMessageRouterConfig config;
			m_msg_router = CefMessageRouterRendererSide::Create(config);
		}

		virtual void OnContextCreated(CefRefPtr<BrowserApp> app,
									  CefRefPtr<CefBrowser> browser, 
									  CefRefPtr<CefFrame> frame, 
									  CefRefPtr<CefV8Context> context) OVERRIDE
		{
			m_msg_router->OnContextCreated(browser, frame, context);
		}

		virtual void OnContextReleased(CefRefPtr<BrowserApp> app,
									   CefRefPtr<CefBrowser> browser, 
									   CefRefPtr<CefFrame> frame, 
									   CefRefPtr<CefV8Context> context) OVERRIDE
		{
			m_msg_router->OnContextReleased(browser, frame, context);
		}

		virtual bool OnProcessMessageReceived(CefRefPtr<BrowserApp> app, 
											  CefRefPtr<CefBrowser> browser, 
											  CefProcessId source_process, 
											  CefRefPtr<CefProcessMessage> message) OVERRIDE
		{
			return m_msg_router->OnProcessMessageReceived(browser, source_process, message);
		}

	private:
		// Handles the renderer side of query routing.
		CefRefPtr<CefMessageRouterRendererSide> m_msg_router;

		IMPLEMENT_REFCOUNTING(BrowserRenderDelegate);
	};

	void CreateRenderDelegates(BrowserRenderSet& delegates)
	{
		delegates.insert(new BrowserRenderDelegate);
	}
} // RenderDelegate