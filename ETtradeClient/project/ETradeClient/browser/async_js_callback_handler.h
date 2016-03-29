#ifndef ETRADECLIENT_BROWSER_ASYNC_JS_CALLBACK_HANDLER_H_INCLUDED
#define ETRADECLIENT_BROWSER_ASYNC_JS_CALLBACK_HANDLER_H_INCLUDED

#include "etradeclient/browser/main_view_browser_handler.h"

namespace AsyncJSCallbackHandler
{
	// Factory method to create the read card handler and add it into the handler set.
	void Create(MainViewBrowserHandler::MessageHandlerSet& handlers);
} // AsyncJSCallbackHandler

#endif // ETRADECLIENT_BROWSER_ASYNC_JS_CALLBACK_HANDLER_H_INCLUDED