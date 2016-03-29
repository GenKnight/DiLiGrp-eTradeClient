#include "stdafx.h"

#include "etradeclient/browser/async_js_callback_handler.h"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/hardware/hardware_cmd.h"
#include "etradeclient/utility/url_regex.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/string_converter.h"

namespace HW
{
	namespace PT = boost::property_tree;

	// Tag & value of the hardware function request & response string in JSON format.
	static const std::string JSON_TAG_CMD = "command";
	static const std::string JSON_TAG_FILEID = "fileId";
	static const std::string JSON_TAG_DATA = "data";
	static const std::string JSON_TAG_SEQ = "seq";
	static const std::string JSON_TAG_ANS = "answer";
	static const std::string JSON_TAG_ERROR_CODE = "errorCode";

	static const std::string JSON_VAL_CMD_ACTIVATE = "activate";
	static const std::string JSON_VAL_CMD_RESET = "reset";
	static const std::string JSON_VAL_CMD_READ = "read";
	static const std::string JSON_VAL_CMD_WRITE = "write";

	static const std::string JSON_VAL_ANS_ACTIVATE_ACK = "activate_ack";
	static const std::string JSON_VAL_ANS_ACTIVATE_FAILED = "activate_failed";
	static const std::string JSON_VAL_ANS_RESET_ACK = "reset_ack";
	static const std::string JSON_VAL_ANS_RESET_FAILED = "reset_failed";
	static const std::string JSON_VAL_ANS_READ_ACK = "read_ack";
	static const std::string JSON_VAL_ANS_READ_FAILED = "read_failed";
	static const std::string JSON_VAL_ANS_WRITE_ACK = "write_ack";
	static const std::string JSON_VAL_ANS_WRITE_FAILED = "write_failed";

	static const std::string JSON_VAL_FILEID_DILI_CARD = "00";
	static const std::string JSON_VAL_FILEID_DILI_CARD_BASIC_INFO = "01";
	static const std::string JSON_VAL_FILEID_DILI_CARD_SERVICE_INFO = "02"; // Service info.
	static const std::string JSON_VAL_FILEID_ID_CARD = "10";
	static const std::string JSON_VAL_FILEID_PIN_PAD = "11";
	static const std::string JSON_VAL_FILEID_BANK_CARD_NUM = "20";

	struct HWRequest
	{
		std::string	cmd;
		std::string file_id;
		std::string data;
		std::string	seq; // The sequence number of the request.
	};

	HWRequest ParseRequest(const std::string& request_json)
	{
		PT::ptree ptree;
		std::stringstream ss;
		ss << request_json;
		PT::read_json(ss, ptree);
		std::string cmd = ptree.get<std::string>(JSON_TAG_CMD);
		std::string file_id = ptree.get<std::string>(JSON_TAG_FILEID);
		std::string data = ptree.get<std::string>(JSON_TAG_DATA);
		std::string seq = ptree.get<std::string>(JSON_TAG_SEQ);
		return { cmd, file_id, data, seq };
	}

	std::string MakeResultJSON(const std::string& answer,
		const std::string& file_id,
		const std::string& error_code,
		const std::string& seq,
		const PT::ptree& data)
	{
		PT::ptree root;
		root.put(JSON_TAG_ANS, answer);
		root.put(JSON_TAG_FILEID, file_id);
		root.put_child(JSON_TAG_DATA, data);
		root.put(JSON_TAG_ERROR_CODE, error_code);
		root.put(JSON_TAG_SEQ, seq);
		std::stringstream ss;
		PT::write_json(ss, root, false);
		return ss.str();
	}

	class HWRequestHandler
	{
		typedef boost::ptr_map<std::string, HardwareCmd> HardwareCmdMap; // Key: cmd+file_id.
	public:
		HWRequestHandler()
		{
			m_hw_cmd_map.insert(JSON_VAL_CMD_READ + JSON_VAL_FILEID_ID_CARD, new ReadIDCardCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_READ + JSON_VAL_FILEID_PIN_PAD, new ReadPINPadCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_ACTIVATE + JSON_VAL_FILEID_DILI_CARD, new ActivateDILICardCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_RESET + JSON_VAL_FILEID_DILI_CARD, new ResetDILICardCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_READ + JSON_VAL_FILEID_DILI_CARD_BASIC_INFO, new ReadDILICardBasicInfoCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_READ + JSON_VAL_FILEID_DILI_CARD_SERVICE_INFO, new ReadDILICardServiceInfoCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_WRITE + JSON_VAL_FILEID_DILI_CARD_SERVICE_INFO, new WriteDILICardServiceInfoCmd());
			m_hw_cmd_map.insert(JSON_VAL_CMD_READ + JSON_VAL_FILEID_BANK_CARD_NUM, new ReadBankCardNumCmd());
		}
		std::string HandleRequest(const std::string& request_json)
		{
			try
			{
				static const std::string OK = ""; // TODO maybe improve this use the same variable defined by "HardwareCmd".
				HWRequest hw_req = ParseRequest(request_json);

				LOG_TRACE(L"执行硬件调用请求:" + str_2_wstr("[" + hw_req.cmd + "," +hw_req.file_id + "," + hw_req.seq + "]"));
				HardwareCmd::Reply reply = m_hw_cmd_map.at(hw_req.cmd + hw_req.file_id).Execute(hw_req.data);
				std::string answer = "";
				if (boost::iequals(OK, reply.error_code)) // Succeed.
					answer = hw_req.cmd + "_ack";
				else
					answer = hw_req.cmd + "_failed";
				std::string reply_json = MakeResultJSON(answer, hw_req.file_id, reply.error_code, hw_req.seq, reply.data);
				LOG_TRACE(L"执行硬件调用结果:" + str_2_wstr("[" + hw_req.cmd + "," + hw_req.file_id + "," + hw_req.seq + "] [" + reply.error_code + "]") + +L"\n");
				return reply_json;
			}
			catch (std::exception& ex)
			{
				LOG_FATAL(L"执行硬件调用请求发生异常，异常信息： " + str_2_wstr(ex.what()) + L"\n");
				return "";
			}
		}
	private:
		HardwareCmdMap m_hw_cmd_map;
	};

	/*Callback handler for call the hardware function.*/
	class Handler : public CefMessageRouterBrowserSide::Handler
	{
	public:
		// This handler function will be called due to 'window.cefQuery' call in web page's JavaScript.
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int64 query_id,
			const CefString& request_json,
			bool persistent,
			CefRefPtr<Callback> callback) OVERRIDE
		{
			// Only handle the message from our own host. This is an important security check!
			CefString url_ = frame->GetURL();
			if (!CheckHost(url_.ToString()))
				return false;
			callback->Success(m_hw_req_handler.HandleRequest(request_json));
			return true;
		}
	private:
		bool CheckHost(const std::string& url_)
		{
			std::smatch match_res;
			if (!URLRegex::Parse(url_, match_res))
				return false;
			return 0 == URLConfig::Instance().Host().compare(match_res[3].str());
		}
	private:
		HWRequestHandler m_hw_req_handler;
	};
} // HW

namespace AsyncJSCallbackHandler
{
	// Handler creation. Called from MainViewBrowserHandler.
	void Create(MainViewBrowserHandler::MessageHandlerSet& handlers)
	{
		//TODO: need exception handling here? return true/false indicator?
		handlers.emplace(new HW::Handler());
	}
} // AsyncJSCallbackHandler