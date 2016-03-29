#ifndef ETRADECLIENT_UTILITY_URL_REGEX_H_INCLUDED
#define ETRADECLIENT_UTILITY_URL_REGEX_H_INCLUDED

#include <regex>
#include <string>
/*
Example:
URL: https://icounter.nong12.com/counter/getUserInfo.do?x=123

Match result:
group #1: https://
group #2: https
group #3: icounter.nong12.com
group #4: /counter/getUserInfo.do
group #5: ?x=123

So, "match_res[3]" stands for the host name & "match_res[4]" stands for the resource path.
*/
namespace URLRegex
{
	/*WARNING: the argument 'url' shouldn't be a temporary varaible, the caller should make sure*/
	inline bool Parse(const std::string& url, std::smatch& match_res)
	{
		static const std::regex kURLRegex("^(([hH][tT][tT][pP][sS]?):\\/\\/)?([^:\\/]+)([\\/\\w\\-\\.]*)([\\?\\w\\.\\=]*)");
		return std::regex_match(url, match_res, kURLRegex);
	}
} // URLRegex

#endif // ETRADECLIENT_UTILITY_URL_REGEX_H_INCLUDED
