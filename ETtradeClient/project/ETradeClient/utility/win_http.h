#ifndef ETRADECLIENT_UTILITY_WIN_HTTP_H_INCLUDED
#define ETRADECLIENT_UTILITY_WIN_HTTP_H_INCLUDED

#include <Windows.h>
#include <winhttp.h>

#include <string>
#include <list>
#include <cstdint>

/*Currently we only support synchronous WinHTTP request!
Notice: the APIs of "WinHttp" may throw exceptions when error happens, these exception messages contain
Chinese characters which are encoding in GBK encoding, so the caller has to call "gbk_2_wstr" to convert the 
exception message to std::wstring in Unicode encoding.
*/

class WinHttp
{
public:
	struct Cookie
	{
		//Currently "expire" is not supported.
		std::string name;
		std::string value;
		std::string domain;
		std::string path;
		bool secure;
		bool httponly;
	};

	enum class Method
	{
		GET = 0,
		POST,
	};

	typedef std::list<Cookie>	Cookies;
	
	/*Movable but non-copyable.*/
	class Request
	{
	public:
		Request() :m_content_type("Content-Type: application/x-www-form-urlencoded; charset=UTF-8"){};
		Request(Request&&);
		Request& operator=(Request&&);
		~Request();
		void SetCookies(const Cookies& cookies);
		void SetClientCertificate(const std::string& cert_store_name, const std::string& subject_name);
		void SetContentType(std::string content_type);
		void SetPostData(const std::string& post_data);
		void Send(); // Send request to server synchronously.

		// Don't worry about the cost of returning a string. because the compiler will most likely optimize out any unnecessary copies.
		// See: https://en.wikipedia.org/wiki/Return_value_optimization
		// Besides, in C++11, move semantics means that you don't perform a full copy even if the compiler doesn't perform RVO.
		// See: https://en.wikipedia.org/wiki/C%2B%2B11#Rvalue_references_and_move_constructors
		std::string ReadResponseHeader() const;
		std::string ReadResponseBody() const;
		uint32_t GetResponseStatus() const;
		Cookies GetCookies() const; // Still, don't worry about the cost because we have RVO & move semantics.
	private:
		Request(HINTERNET handle, const Method& method);
		Request(const Request&);
		Request& operator=(const Request&);

		void Get();
		void Post();

		// Refer to: https://msdn.microsoft.com/en-us/library/aa384070%28v=vs.85%29.aspx
		void QuerySingleHeader(std::wstring& header, uint32_t query_info_flag, DWORD* header_idx) const;
		void QueryMultiHeader(std::list<std::wstring>& header_list, uint32_t query_info_flag) const;
		void ParseCookies(const std::list<std::wstring>& header_list, Cookies& cookies) const;

		void Close();
	private:
		HINTERNET		m_request;
		Method			m_method;
		std::string		m_content_type;
		std::string		m_post_data;

		friend class WinHttp;
	};

	/*This constructor may throw exception if system error happens such as no enough memory or other internal error.*/
	WinHttp();
	~WinHttp();

	/*Throw exception if connecting failed.*/
	void ConnectHost(const std::string& host, uint16_t port, bool is_https = false);
	void DisconnectHost();

	Request OpenRequest(const Method& method, const std::string& res_path);
	void CloseRequest(Request& req); // Call this method will release the request resource and set is as NULL.
private:
	bool			m_is_https;
	HINTERNET		m_session;
	HINTERNET		m_connect;
};


#endif // ETRADECLIENT_UTILITY_WIN_HTTP_H_INCLUDED
