#include "stdafx.h"

#include "etradeclient/browser/url_request_client.h"

URLRequestClient::URLRequestClient(const Callback& callback) : m_callback(callback)
{
	CEF_REQUIRE_UI_THREAD();
	DCHECK(!m_callback.is_null());
}

void URLRequestClient::Detach()
{
	CEF_REQUIRE_UI_THREAD();
	if (!m_callback.is_null())
		m_callback.Reset();
}

void URLRequestClient::OnRequestComplete(CefRefPtr<CefURLRequest> request)
{
	CEF_REQUIRE_UI_THREAD();
	if (!m_callback.is_null())
	{
		m_callback.Run(request->GetRequestError(), m_download_data);
		m_callback.Reset();
	}
}

void URLRequestClient::OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total)
{}

void URLRequestClient::OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total)
{}

void URLRequestClient::OnDownloadData(CefRefPtr<CefURLRequest> request, const void* data, size_t data_length)
{
	CEF_REQUIRE_UI_THREAD();
	m_download_data += std::string(static_cast<const char*>(data), data_length);
}

bool URLRequestClient::GetAuthCredentials(bool isProxy,
										  const CefString& host,
										  int port,
										  const CefString& realm,
										  const CefString& scheme,
										  CefRefPtr<CefAuthCallback> callback)
{
	return false;
}
