#include "stdafx.h"

#include "etradeclient/browser/user_msg_monitor.h"

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/utility/session.h"
#include "etradeclient/browser/url_request_proxy.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/win_msg_define.h"
#include "etradeclient/utility/string_converter.h"

namespace
{
	static const std::string JSON_TAG_CODE = "code";
	static const std::string JSON_TAG_MSG = "message";
	static const std::string JSON_TAG_AMOUNT = "amount";
	static const std::string JSON_VAL_SUCC = "success";
}

UserMsgMonitor::UserMsgMonitor() : m_continue(false), m_timer(m_io)
{
	m_interval = ApplicationConfig::Instance().MsgCountQueryInterval();
	const uint32_t kIntervalMin = 60, kIntervalMax = 300;
	if (m_interval < kIntervalMin)
		m_interval = kIntervalMin;
	else if (m_interval > kIntervalMax)
		m_interval = kIntervalMax;
}

UserMsgMonitor::~UserMsgMonitor()
{
	Stop();
	m_io.stop(); // Stop io service before destruction.
}

void UserMsgMonitor::Start()
{
	m_continue = true;
	m_thread = std::thread(&UserMsgMonitor::GetMsgCountProc, this);
}

void UserMsgMonitor::Stop()
{
	m_continue = false; // Make sure set flag first and then cancel the timer, this can prevent the chance of entering the loop again.
	m_timer.cancel();
	if (m_thread.joinable())
		m_thread.join();
}

void UserMsgMonitor::CheckOnce(WinHttp::Request& request)
{	
	uint32_t msg_count = 0; // First initialize the value of msg count, this value will be updated only if no error happens.
	do 
	{
		std::stringstream ss;
		const int kDelayMS = 500;
		const int kRequestTimes = 2; // 2 is enough.
		int index = 0;
		for (int index = 0; index < kRequestTimes; ++index)
		{
			try
			{
				const uint32_t kHTTPOK = 200;
				request.Send();
				uint32_t status_code = request.GetResponseStatus();
				if (kHTTPOK != status_code)
				{
					std::string err_msg = "网络请求错误！ 错误码: " + std::to_string(status_code);
					throw std::exception(err_msg.c_str());
				}
				std::string response_body = request.ReadResponseBody();
				if (response_body.empty())
					throw std::exception("获取服务器响应数据失败，请确保网络连接正常");
				ss << response_body;
				break;
			}
			catch (std::exception& ex)
			{
				LOG_ERROR(gbk_2_wstr(ex.what()));
				std::this_thread::sleep_for(std::chrono::milliseconds(kDelayMS));
				continue;
			}
		}
		if (kRequestTimes == index)
		{
			LOG_ERROR(L"从服务器获取未读通知消息数量失败！");
			break;
		}
		namespace PT = boost::property_tree;
		bool login_res = false;
		try //Parse the configuration file
		{
			PT::ptree ptree;
			PT::read_json(ss, ptree);
			std::string res = ptree.get<std::string>(JSON_TAG_CODE);
			login_res = res.compare(JSON_VAL_SUCC) == 0 ? true : false;
			if (!login_res)
			{
				LOG_ERROR(L"服务器处理失败，返回信息：" + str_2_wstr(res) + str_2_wstr(ptree.get<std::string>(JSON_TAG_MSG)));
				return;
			}
			else
#if _DEBUG
				msg_count = 1; // @TODO remove
#else
				msg_count = ptree.get<uint32_t>(JSON_TAG_AMOUNT);
#endif
		}
		catch (...)
		{
			LOG_ERROR(L"解析服务器返回的未读通知消息数量时出错！请确认返回数据不为空，返回的数据格式为正确的Json格式！");
			return;
		}
	} while (0);
	// Post message to CMainFrame to update msg count.
	// AfxGetMainWnd() will return NULL if called from an other thread (has to do with thread local storage).
	// That's why we use AfxGetApp()->GetMainWnd() instead of AfxGetMainWnd().
	PostMessageW(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_UPDATE_USER_MSG_COUNT, (WPARAM)msg_count, NULL);
}

void UserMsgMonitor::GetMsgCountProc()
{
	try
	{
		const uint32_t kHTTPOK = 200;
		std::wstring err_msg = L"";
		auto& url_cfg = URLConfig::Instance();
		WinHttp win_http;
		win_http.ConnectHost(url_cfg.Host(), url_cfg.Port(), url_cfg.IsHttps());
		auto request = win_http.OpenRequest(WinHttp::Method::GET, url_cfg.UserMsgCountPath());
		if (url_cfg.IsHttps())
		{
			auto& app_cfg = ApplicationConfig::Instance();
			request.SetClientCertificate(app_cfg.ClientCertStore(), app_cfg.ClientCertSubject());
		}
			
		request.SetCookies(Session::Instance().Cookies());

		CheckOnce(request); // Check once before waiting.
		while (m_continue)
		{
			m_timer.expires_from_now(boost::posix_time::seconds(m_interval));
			m_timer.async_wait(
				[&](const boost::system::error_code& ec)
			{
				if (!ec)
					CheckOnce(request);
			});
			m_io.run_one();
			m_io.reset();
		}
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(gbk_2_wstr(ex.what()));
	}
}