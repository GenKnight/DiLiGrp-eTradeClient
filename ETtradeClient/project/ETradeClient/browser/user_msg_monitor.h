#ifndef ETRADECLIENT_BROWSER_USER_MSG_MONITOR_H_INCLUDED
#define ETRADECLIENT_BROWSER_USER_MSG_MONITOR_H_INCLUDED

#include <thread>
#include <atomic>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "etradeclient/utility/win_http.h"

class UserMsgMonitor
{
public:
	UserMsgMonitor();
	~UserMsgMonitor();

	void Start();
	void Stop();
private:
	void CheckOnce(WinHttp::Request& request); // Request the msg count once.
	void GetMsgCountProc();

	std::thread			m_thread;
	std::atomic<bool>	m_continue;
	uint32_t			m_interval;

	boost::asio::io_service		m_io; // Make sure m_io is declared before m_timer!
	boost::asio::deadline_timer m_timer;
};

#endif // ETRADECLIENT_BROWSER_USER_MSG_MONITOR_H_INCLUDED
