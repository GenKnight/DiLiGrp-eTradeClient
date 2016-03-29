#ifndef ETRADECLIENT_BROWSER_URL_REQUEST_CLIENT_H_INCLUDED
#define ETRADECLIENT_BROWSER_URL_REQUEST_CLIENT_H_INCLUDED

#include <cstdint>
#include <string>

#include "include/cef_urlrequest.h"
#include "include/wrapper/cef_helpers.h"

// Implementation of CefURLRequestClient that stores response information. Only
// accessed on the UI thread.

class URLRequestClient : public CefURLRequestClient
{
public:
	// Callback to be executed on request completion.
	typedef base::Callback<void(CefURLRequest::ErrorCode /*error_code*/, const std::string& /*download_data*/)> Callback;

	explicit URLRequestClient(const Callback& callback);

	void Detach();
	void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE;
	void OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) OVERRIDE;
	void OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) OVERRIDE;
	void OnDownloadData(CefRefPtr<CefURLRequest> request, const void* data, size_t data_length) OVERRIDE;
	bool GetAuthCredentials(bool isProxy,
							const CefString& host,
							int port,
							const CefString& realm,
							const CefString& scheme,
							CefRefPtr<CefAuthCallback> callback) OVERRIDE;
private:
	Callback		m_callback;
	std::string		m_download_data;

	IMPLEMENT_REFCOUNTING(URLRequestClient);
	DISALLOW_COPY_AND_ASSIGN(URLRequestClient);
};

#endif // ETRADECLIENT_BROWSER_URL_REQUEST_CLIENT_H_INCLUDED
