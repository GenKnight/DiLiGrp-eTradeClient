#include "stdafx.h"

#include "etradeclient/browser/url_request_proxy.h"

#include <functional>

#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"

#include "etradeclient/browser/url_request.h"
#include "etradeclient/utility/logging.h"


URLRequestProxy::URLRequestProxyImpl::URLRequestProxyImpl(std::mutex& mtx, std::condition_variable& cv) :
	m_mtx(mtx), m_cv(cv), m_req_sent(false), m_res(false), m_response_str("")
{}
void URLRequestProxy::URLRequestProxyImpl::Get(const std::string& request_url, int flags)
{
	CEF_REQUIRE_UI_THREAD();
	CefRefPtr<CefRequest> request = CefRequest::Create();
	request->Set(request_url, "GET", nullptr, CefRequest::HeaderMap());
	request->SetFlags(flags);
	SendRequest(request);
}
void URLRequestProxy::URLRequestProxyImpl::Post(const std::string& request_url, const std::string& request_param, int flags)
{
	CEF_REQUIRE_UI_THREAD();
	CefRefPtr<CefRequest> request = CefRequest::Create();
	CefRefPtr<CefPostData> postData = CefPostData::Create();
	CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
	element->SetToBytes(request_param.size(), request_param.c_str());
	postData->AddElement(element);
	request->Set(request_url, "POST", postData, CefRequest::HeaderMap());
	request->SetFlags(flags);
	SendRequest(request);
}
void URLRequestProxy::URLRequestProxyImpl::Cancel()
{
	CEF_REQUIRE_UI_THREAD();
	if (nullptr != m_url_request.get())
		m_url_request->CancelPendingRequest();
	m_url_request.reset(nullptr);
	m_cv.notify_one();
}
bool URLRequestProxy::URLRequestProxyImpl::GotResponse() const
{
	return m_req_sent;
}
bool URLRequestProxy::URLRequestProxyImpl::Result() const
{
	return m_res;
}
const std::string& URLRequestProxy::URLRequestProxyImpl::ResponseString() const
{
	return m_response_str;
}

void URLRequestProxy::URLRequestProxyImpl::SendRequest(CefRefPtr<CefRequest> request)
{
	m_req_sent = false;
	m_res = false;
	m_response_str.swap(std::string(""));
	if (nullptr == m_url_request.get())
		m_url_request.reset(new URLRequest());
	m_url_request->Send(request, 
		std::bind(&URLRequestProxyImpl::HandleResponse, this, std::placeholders::_1, std::placeholders::_2));
}
void URLRequestProxy::URLRequestProxyImpl::HandleResponse(CefURLRequest::ErrorCode error_code, const std::string& response_str)
{
	CEF_REQUIRE_UI_THREAD();
	{
		std::lock_guard<std::mutex> lg(m_mtx);
		m_req_sent = true;
		m_res = (ERR_NONE == error_code);
		m_response_str = response_str;
		if (!m_res)
			LOG_ERROR(L"CEFÕ¯¬Á«Î«Û ß∞‹£°¥ÌŒÛ¥˙¬Î: " + std::to_wstring(error_code));
		m_url_request.reset(nullptr);
	}
	m_cv.notify_one();
}

URLRequestProxy::URLRequestProxy() : m_pimpl(new URLRequestProxyImpl(m_mtx, m_cv))
{}

bool URLRequestProxy::Get(const std::string& request_url, int flags)
{
	CefPostTask(TID_UI, base::Bind(&URLRequestProxyImpl::Get, m_pimpl, request_url, flags));

	std::unique_lock<std::mutex> lk(m_mtx);
	m_cv.wait(lk, [&]
	{
		return m_pimpl->GotResponse();
	});
	return m_pimpl->Result();
}

bool URLRequestProxy::Post(const std::string& request_url, const std::string& request_param, int flags)
{
	CefPostTask(TID_UI, base::Bind(&URLRequestProxyImpl::Post, m_pimpl, request_url, request_param, flags));

	std::unique_lock<std::mutex> lk(m_mtx);
	m_cv.wait(lk, [&]
	{
		return m_pimpl->GotResponse();
	});
	return m_pimpl->Result();
}

const std::string& URLRequestProxy::ResponseString() const
{
	return m_pimpl->ResponseString();
}