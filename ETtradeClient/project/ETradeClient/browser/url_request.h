#ifndef ETRADECLIENT_BROWSER_URL_REQUEST_H_INCLUDED
#define ETRADECLIENT_BROWSER_URL_REQUEST_H_INCLUDED

/*
Applications can send network requests not associated with a particular browser via the CefURLRequest class. 
Implement the CefURLRequestClient interface to handle the resulting response. 
CefURLRequest can be used in both the browser and render processes.
*/

#include <string>
#include <functional>

#include "include/cef_urlrequest.h"

class URLRequest
{
public:
	typedef std::function<void(CefURLRequest::ErrorCode /*error_code*/, const std::string& /*download_data*/)> Callback;

	URLRequest();
	~URLRequest();

	void Send(CefRefPtr<CefRequest> request, const Callback& handler);
	void CancelPendingRequest();// Cancel the currently pending URL request, if any.

private:
	void OnRequestComplete(CefURLRequest::ErrorCode error_code, const std::string& download_data);

private:
	Callback				 m_handler;
	CefRefPtr<CefURLRequest> m_cef_url_request;

	DISALLOW_COPY_AND_ASSIGN(URLRequest);
};

#endif // ETRADECLIENT_BROWSER_URL_REQUEST_H_INCLUDED
