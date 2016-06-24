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
#include "etradeclient/utility/win_msg_define.h"

namespace
{
	namespace PT = boost::property_tree;

	// Tag & value of the hardware function request & response string in JSON format.
	static const std::string JSON_TAG_CMD = "command";
	static const std::string JSON_TAG_FILEID = "fileId";
	static const std::string JSON_TAG_DATA = "data";
	static const std::string JSON_TAG_SEQ = "seq";
	static const std::string JSON_TAG_ANS = "answer";
	static const std::string JSON_TAG_ERROR_CODE = "errorCode";

	struct JSRequest
	{
		std::string	cmd;
		std::string file_id;
		std::string data;
		std::string	seq; // The sequence number of the request.
	};

	JSRequest ParseRequest(const std::string& request_json)
	{
		PT::ptree ptree;
		std::stringstream ss;
		ss << request_json;
		PT::read_json(ss, ptree);
		std::string cmd = ptree.get<std::string>(JSON_TAG_CMD);
		std::string file_id = ptree.get<std::string>(JSON_TAG_FILEID);
		std::string data = ptree.get<std::string>(JSON_TAG_DATA);
		std::string seq = ptree.get<std::string>(JSON_TAG_SEQ);
		return{ cmd, file_id, data, seq };
	}

	std::string MakeResultJSON(const std::string& answer,
		const std::string& file_id,
		const std::string& error_code,
		const std::string& seq,
		const PT::ptree& data = PT::ptree())
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
	bool IsEqual(const std::string& str1, const std::string& str2)
	{
		return 0 == str1.compare(str2);
	}
}


namespace HW
{
	// Tag & value of the hardware function request & response string in JSON format.

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
				JSRequest hw_req = ParseRequest(request_json);

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
} // HW


namespace UI{
	// Tag & value of the hardware function request & response string in JSON format.

	static const std::string JSON_VAL_CMD_CLOSE_WND = "close_wnd";

	static const std::string JSON_VAL_ANS_CLOSE_WND_ACK = "close_wnd_ack";
	static const std::string JSON_VAL_ANS_CLOSE_WND_FAILED = "close_wnd_failed";

	static const std::string JSON_VAL_FILEID_CREAT_MERCHANT_VIEW = "30";

	static const std::string OK = ""; // 操作成功.
	static const std::string ERR = "601"; // 操作失败.

	std::string HandleRequest(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& request_json)
	{
		JSRequest js_rq = ParseRequest(request_json);
		
		if (IsEqual(js_rq.cmd, JSON_VAL_CMD_CLOSE_WND)
			&& IsEqual(js_rq.file_id, JSON_VAL_FILEID_CREAT_MERCHANT_VIEW))
		{
			::PostMessage(browser->GetHost()->GetWindowHandle(), WM_CLOSE, NULL, NULL);
			return MakeResultJSON(JSON_VAL_ANS_CLOSE_WND_ACK, js_rq.file_id, OK, js_rq.seq);
		}

		return MakeResultJSON(JSON_VAL_ANS_CLOSE_WND_FAILED, js_rq.file_id, ERR, js_rq.seq);
	}
}

namespace{
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
#ifndef _TEST
			// Only handle the message from our own host. This is an important security check!
			CefString url_ = frame->GetURL();
			if (!CheckHost(url_.ToString()))
				return false;
#endif
			Handing(browser, frame, request_json, callback);
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
		virtual void Handing(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& request_json,
			CefRefPtr<Callback> callback) = 0;
	};

	/*Callback handler for call the hardware function.*/
	class HWHandle :public Handler
	{
	private:
		void Handing(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& request_json,
			CefRefPtr<Callback> callback)
		{
			callback->Success(m_hw_req_handler.HandleRequest(request_json));
		}
	private:
		HW::HWRequestHandler m_hw_req_handler;
	};

	class UIHandle :public Handler
	{
	private:
		void Handing(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& request_json,
			CefRefPtr<Callback> callback)
		{
			callback->Success(UI::HandleRequest(browser, frame, request_json));
		}
	private:
		HW::HWRequestHandler m_hw_req_handler;
	};
}



namespace AsyncJSCallbackHandler
{
	// Handler creation. Called from MainViewBrowserHandler.
	void HW_Create(MainViewBrowserHandler::MessageHandlerSet& handlers)
	{
		//TODO: need exception handling here? return true/false indicator?
		handlers.emplace(new HWHandle());
	}

	// Handler creation. Called from PopupBrowserHandler(CreateMerchantView).
	void UI_Create(PopupBrowserHandler::MessageHandlerSet& handlers)
	{
		//TODO: need exception handling here? return true/false indicator?
		handlers.emplace(new UIHandle());
	}

} // AsyncJSCallbackHandler