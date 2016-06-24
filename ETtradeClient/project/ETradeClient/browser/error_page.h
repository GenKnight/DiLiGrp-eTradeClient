#ifndef ETRADECLIENT_BROWSER_ERROR_PAGE_H_INCLUDED
#define ETRADECLIENT_BROWSER_ERROR_PAGE_H_INCLUDED

#include <string>

#include "include/internal/cef_types.h"

namespace ErrorPage{
	std::string ToErrorString(cef_errorcode_t code);
	std::wstring Url(const std::string& failed_url, cef_errorcode_t error_code, const std::string& other_info);
}

#endif  // ETRADECLIENT_BROWSER_ERROR_PAGE_H_INCLUDED
