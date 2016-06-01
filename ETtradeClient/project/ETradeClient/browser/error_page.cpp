#include "stdafx.h"

#include "error_page.h"

#include "include/cef_parser.h"

namespace ErrorPage{

	std::string ToErrorString(cef_errorcode_t code)
	{
		// Case condition that returns |code| as a string.
		#define CASE(code) case code: return #code

		switch (code) {
			CASE(ERR_NONE);
			CASE(ERR_FAILED);
			CASE(ERR_ABORTED);
			CASE(ERR_INVALID_ARGUMENT);
			CASE(ERR_INVALID_HANDLE);
			CASE(ERR_FILE_NOT_FOUND);
			CASE(ERR_TIMED_OUT);
			CASE(ERR_FILE_TOO_BIG);
			CASE(ERR_UNEXPECTED);
			CASE(ERR_ACCESS_DENIED);
			CASE(ERR_NOT_IMPLEMENTED);
			CASE(ERR_CONNECTION_CLOSED);
			CASE(ERR_CONNECTION_RESET);
			CASE(ERR_CONNECTION_REFUSED);
			CASE(ERR_CONNECTION_ABORTED);
			CASE(ERR_CONNECTION_FAILED);
			CASE(ERR_NAME_NOT_RESOLVED);
			CASE(ERR_INTERNET_DISCONNECTED);
			CASE(ERR_SSL_PROTOCOL_ERROR);
			CASE(ERR_ADDRESS_INVALID);
			CASE(ERR_ADDRESS_UNREACHABLE);
			CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
			CASE(ERR_TUNNEL_CONNECTION_FAILED);
			CASE(ERR_NO_SSL_VERSIONS_ENABLED);
			CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
			CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
			CASE(ERR_CERT_COMMON_NAME_INVALID);
			CASE(ERR_CERT_DATE_INVALID);
			CASE(ERR_CERT_AUTHORITY_INVALID);
			CASE(ERR_CERT_CONTAINS_ERRORS);
			CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
			CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
			CASE(ERR_CERT_REVOKED);
			CASE(ERR_CERT_INVALID);
			CASE(ERR_CERT_END);
			CASE(ERR_INVALID_URL);
			CASE(ERR_DISALLOWED_URL_SCHEME);
			CASE(ERR_UNKNOWN_URL_SCHEME);
			CASE(ERR_TOO_MANY_REDIRECTS);
			CASE(ERR_UNSAFE_REDIRECT);
			CASE(ERR_UNSAFE_PORT);
			CASE(ERR_INVALID_RESPONSE);
			CASE(ERR_INVALID_CHUNKED_ENCODING);
			CASE(ERR_METHOD_NOT_SUPPORTED);
			CASE(ERR_UNEXPECTED_PROXY_AUTH);
			CASE(ERR_EMPTY_RESPONSE);
			CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
			CASE(ERR_CACHE_MISS);
			CASE(ERR_INSECURE_RESPONSE);
		default:
			return "UNKNOWN";
		}
	}

	std::wstring Url(const std::string& failed_url, cef_errorcode_t error_code, const std::string& other_info)
	{
		std::stringstream ss;
		ss << "<html><head><title>ERROR!</title></head>"
			"<body bgcolor=\"white\">"
			"<h3>Page failed to load.</h3>"
			"<br/>Error: " << ToErrorString(error_code) <<
			" (" << error_code << ")";
		if (!other_info.empty())
			ss << "<br/>" << other_info;
		ss << "</body></html>";

		std::string html_data = ss.str();
		CefString data = CefBase64Encode(html_data.data(), html_data.size());
		std::wstring err_report_url = L"data:text/html;base64," +
			CefURIEncode(data, false).ToWString();
		return err_report_url;
	}
}