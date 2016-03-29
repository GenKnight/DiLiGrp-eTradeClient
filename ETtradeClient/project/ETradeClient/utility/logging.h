#ifndef ETRADECLIENT_UTILITY_LOGGING_H_INCLUDED
#define ETRADECLIENT_UTILITY_LOGGING_H_INCLUDED

#include <string>
#include <cstdint>
#include <boost/log/trivial.hpp>

namespace Logging
{
	namespace trivial = boost::log::trivial;

	enum class Level
	{
		kTrace = trivial::trace,
		kDebug = trivial::debug,
		kInfo = trivial::info,
		kWarning = trivial::warning,
		kError = trivial::error,
		kFatal = trivial::fatal
	};

	void Init(const Level& filter_level, const std::string& log_file_name_prefix, uint32_t max_log_file_size_kb);
	void Log(const Level& message_level, const std::wstring& message);
} // Logging

// Here this macro will add "FILE" "FUNCTION" and "LINE" infomation.
#define LOCATION	L"{FILE:" + std::wstring(__FILEW__) + \
					L" LINE:" + std::to_wstring(__LINE__) + \
					L" FUNC:" + std::wstring(__FUNCTIONW__) + \
					L"} "

#define LOG_TRACE(Messages)	   Logging::Log(Logging::Level::kTrace, Messages)
#define LOG_DEBUG(Messages)	   Logging::Log(Logging::Level::kDebug, Messages)
#define LOG_INFO(Messages)	   Logging::Log(Logging::Level::kInfo, Messages)
#define LOG_WARNING(Messages)  Logging::Log(Logging::Level::kWarning, Messages)
#define LOG_ERROR(Messages)	   Logging::Log(Logging::Level::kError, Messages)
#define LOG_FATAL(Messages)	   Logging::Log(Logging::Level::kFatal, Messages)

/*
inline std::wstring LogLocation()
{
	return L"{FILE:" + std::wstring(__FILEW__) + 
		L" LINE:" + std::to_wstring(__LINE__) + 
		L" FUNC:" + std::wstring(__FUNCTIONW__) 
		+ L"} ";
}
inline void LogTrace(const std::wstring& msg)	{ Logging::Log(Logging::Level::kTrace, msg); }
inline void LogDebug(const std::wstring& msg)	{ Logging::Log(Logging::Level::kDebug, msg); }
inline void LogInfo(const std::wstring& msg)	{ Logging::Log(Logging::Level::kInfo, msg); }
inline void LogWarning(const std::wstring& msg) { Logging::Log(Logging::Level::kWarning, msg); }
inline void LogError(const std::wstring& msg)	{ Logging::Log(Logging::Level::kError, msg); }
inline void LogFatal(const std::wstring& msg)	{ Logging::Log(Logging::Level::kFatal, msg); }
*/

#endif // ETRADECLIENT_UTILITY_LOGGING_H_INCLUDED
