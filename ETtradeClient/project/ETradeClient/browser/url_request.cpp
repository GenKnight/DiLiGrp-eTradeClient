#include "stdafx.h"

#include "etradeclient/browser/url_request.h"

#include "include/base/cef_bind.h"
#include "include/base/cef_callback.h"
#include "include/wrapper/cef_helpers.h"

#include "etradeclient/browser/url_request_client.h"

URLRequest::URLRequest()
{
	CEF_REQUIRE_UI_THREAD();
}

URLRequest::~URLRequest()
{
	CEF_REQUIRE_UI_THREAD();
}

void URLRequest::Send(CefRefPtr<CefRequest> request, const Callback& handler)
{
	CEF_REQUIRE_UI_THREAD();

	m_handler = handler;
	CancelPendingRequest(); // First cancel pending request.

	// Callback to be executed on request completion.
	// It's safe to use base::Unretained() here because there is only one
	// URLRequestClient pending at any given time and we explicitly detach the
	// callback in the Handler destructor.
	const URLRequestClient::Callback& callback = base::Bind(&URLRequest::OnRequestComplete, base::Unretained(this));
	// The callback will be invoked after response from server being received.
	m_cef_url_request = CefURLRequest::Create(request, new URLRequestClient(callback), NULL); // Create and start the new CefURLRequest.	
}

// Cancel the currently pending URL request, if any.
void URLRequest::CancelPendingRequest()
{
	CEF_REQUIRE_UI_THREAD();

	if (m_cef_url_request.get())
	{
		// Don't execute the callback when we explicitly cancel the request.
		static_cast<URLRequestClient*>(m_cef_url_request->GetClient().get())->Detach();

		m_cef_url_request->Cancel();
		m_cef_url_request = NULL;
	}
}

void URLRequest::OnRequestComplete(CefURLRequest::ErrorCode error_code, const std::string& download_data)
{
	CEF_REQUIRE_UI_THREAD();
	m_cef_url_request = NULL;
	m_handler(error_code, download_data);
}