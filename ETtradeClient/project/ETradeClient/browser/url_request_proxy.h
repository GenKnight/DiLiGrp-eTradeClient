#ifndef ETRADECLIENT_BROWSER_URL_REQUEST_PROXY_H_INCLUDED
#define ETRADECLIENT_BROWSER_URL_REQUEST_PROXY_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <string>

#include "include/internal/cef_ptr.h"
#include "include/cef_urlrequest.h"

class URLRequest; // Forward declaration.

class URLRequestProxy
{
	class URLRequestProxyImpl : public CefBase
	{
	public:
		URLRequestProxyImpl::URLRequestProxyImpl(std::mutex& mtx, std::condition_variable& cv);
		void Get(const std::string& request_url, int flags);
		void Post(const std::string& request_url, const std::string& request_param, int flags);
		void Cancel();
		bool GotResponse() const;
		bool Result() const;
		const std::string& ResponseString() const;
	private:
		void SendRequest(CefRefPtr<CefRequest> request);
		void HandleResponse(CefURLRequest::ErrorCode error_code, const std::string& response_str);

	private:
		std::unique_ptr<URLRequest>		m_url_request;
		std::mutex&						m_mtx;
		std::condition_variable&		m_cv;
		bool							m_req_sent;
		bool							m_res;
		std::string						m_response_str; // Result string of request.

		IMPLEMENT_REFCOUNTING(URLRequestProxyImpl);
		DISALLOW_COPY_AND_ASSIGN(URLRequestProxyImpl);
	};
public:
	URLRequestProxy();
	bool Get(const std::string& request_url, int flags);
	bool Post(const std::string& request_url, const std::string& request_param, int flags);
	const std::string& ResponseString() const;
private:
	std::mutex						m_mtx;
	std::condition_variable			m_cv;
	CefRefPtr<URLRequestProxyImpl>	m_pimpl;
};

#endif // ETRADECLIENT_BROWSER_URL_REQUEST_PROXY_H_INCLUDED
