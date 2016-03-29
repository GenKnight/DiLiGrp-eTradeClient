#include "stdafx.h"

#include "etradeclient/utility/logging.h"

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/locale.hpp>

namespace Logging
{
	namespace sinks = boost::log::sinks;
	namespace attrs = boost::log::attributes;
	namespace expr = boost::log::expressions;
	namespace keywords = boost::log::keywords;

	void Init(const Level& filter_level, const std::string& log_file_name_prefix, uint32_t max_log_file_size_kb)
	{
		using namespace boost::log::aux::default_attribute_names;

		static const std::string kLogFileFolder("Log");
		static const std::string kFileNamePostfix("_%Y%m%d-%2N.log");
		static const uint32_t kFileSizeByte = max_log_file_size_kb * 1024;

		// DO NOT remove this statement otherwise application crashes on process termination.
		// For more detail: http://www.boost.org/doc/libs/1_57_0/libs/log/doc/html/log/rationale/why_crash_on_term.html.
		boost::filesystem::path::imbue(std::locale("C"));

		boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
			keywords::file_name = kLogFileFolder + "/" + log_file_name_prefix + kFileNamePostfix,
			keywords::rotation_size = kFileSizeByte);

		backend->set_file_collector(sinks::file::make_collector(
			keywords::target = kLogFileFolder,
			keywords::max_size = kFileSizeByte));
		backend->scan_for_files();

		backend->auto_flush(true); // Enable auto-flushing after each log record written.

		// Wrap it into the frontend and register in the core. The backend requires synchronization in the frontend.
		typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
		boost::shared_ptr<sink_t> sink(new sink_t(backend));

		// Set format to the front-end.
		sink->set_formatter(expr::format("%1% : [%2%] <%3%> %4%")
			% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
			% expr::attr<attrs::current_thread_id::value_type>(thread_id())
			% trivial::severity
			% expr::message);

		// Support logging wide character.
		std::locale loc = boost::locale::generator()("en_US.UTF-8");
		sink->imbue(loc);

		boost::log::add_common_attributes();
		boost::shared_ptr<boost::log::core> core = boost::log::core::get();
		core->set_filter(trivial::severity >= static_cast<trivial::severity_level>(filter_level));
		core->add_sink(sink);
	}

	void Log(const Level& message_level, const std::wstring& message)
	{
		static boost::log::sources::wseverity_logger<trivial::severity_level> logger;
		BOOST_LOG_SEV(logger, static_cast<trivial::severity_level>(message_level)) << message;
	}
} // Logging