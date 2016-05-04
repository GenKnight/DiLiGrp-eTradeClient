#include "stdafx.h"

#include "etradeclient/hardware/hardware_cmd.h"

#include <sstream>

#include <boost/algorithm/string.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/string_converter.h"
#include "etradeclient/utility/application_config.h"

#include "etradeclient/hardware/pin_pad.h"
#include "etradeclient/hardware/id_card_reader.h"
#include "etradeclient/hardware/dili_card_device.h"
#include "etradeclient/hardware/bank_card_reader.h"
#include "etradeclient/hardware/password_machine.h"

namespace StatusCode
{
	static const std::string OK = ""; // 操作成功.

	static const std::string PINPAD_CONNECT_FAILED = "100"; // 连接密码键盘失败. 
	static const std::string PINPAD_TIMEOUT = "101"; // 用户密码输入超时. 
	static const std::string PINPAD_CANCELLED = "102"; // 用户取消密码输入. 
	static const std::string PINPAD_READ_FAILED = "103"; // 读取密码失败. 

	static const std::string ID_CARD_READER_CONNECT_FAILED = "200"; // 连接身份证读卡器失败. 
	static const std::string ID_CARD_READER_VERIFY_FAILED = "201"; // 身份证验证失败.
	static const std::string ID_CARD_READER_READ_FAILED = "202"; // 读取身份证信息失败.

	static const std::string CPU_CARD_DEVICE_CONNECT_FAILED = "300"; // 打开CPU卡读卡器失败. 
	static const std::string CPU_CARD_DEVICE_CANNOT_FIND_CARD = "301"; // 无法找到卡片，请放卡到读卡器上(或重新放置卡片).
	
	static const std::string DILI_CARD_DEVICE_ACTIVATE_CARD_FAILED = "302"; // 卡片激活失败, 请检查该卡是否已处于激活状态.
	static const std::string DILI_CARD_DEVICE_RESET_CARD_FAILED = "303"; // 卡片撤销激活失败, 请检查该卡是否已处于未激活状态.
	static const std::string DILI_CARD_DEVICE_READ_BASIC_INFO_FAILED = "304"; // 读取卡片基本信息失败.
	static const std::string DILI_CARD_DEVICE_READ_SERVICE_INFO_FAILED = "305"; // 读取卡片业务信息失败.
	static const std::string DILI_CARD_DEVICE_WRITE_SERVICE_INFO_FAILED = "306"; // 写入卡片业务信息失败.
	static const std::string DILI_CARD_DEVICE_WRITE_UNMATCHED_CARD = "307"; // 写卡卡片的序列号不匹配.

	static const std::string PWD_MACHINE_CONNECT_FAILED = "401"; // 连接加密机失败.
	static const std::string PWD_MACHINE_GET_KEY_FAILED = "402"; // 从加密机读取密钥失败.

	static const std::string BANK_CARD_READ_CARD_NUM_FAILED = "501"; // 读取银行卡卡号失败.
}

namespace 
{
	const PINPad& PINPad_()
	{
		static PINPad pin_pad;
		return pin_pad;
	}

	const IDCardReader& IDCardReader_()
	{
		static IDCardReader id_card_reader;
		return id_card_reader;
	}

	const DILICard::RWDevice& DILICardRWDevice()
	{
		static DILICard::RWDevice dili_card_device;
		return dili_card_device;
	}

	const PWDMachine& PWDMachine_()
	{
		static PWDMachine pwd_machine;
		return pwd_machine;
	}

	const BankCardReader& BankCardReader_()
	{
		static BankCardReader bank_card_reader;
		return bank_card_reader;
	}
}

ReadPINPadCmd::Reply ReadPINPadCmd::Execute(const std::string& input)
{
	LOG_TRACE(L"读取用户输入密码。");
	const uint8_t PWD_MAX_LEN = ApplicationConfig::Instance().PINPadPWDLen();
	const uint8_t TIMEOUT = ApplicationConfig::Instance().PINPadTimeout();
	auto& pin_pad = PINPad_();
	std::string pwd("");
	Reply reply;
	do
	{
		try
		{
			if (!pin_pad.Connect())
			{
				LOG_ERROR(L"连接密码键盘失败。");
				reply.error_code = StatusCode::PINPAD_CONNECT_FAILED;
				break;
			}
			pin_pad.DisplayBuiltInText(PINPad::BuiltInText::kEnterPWD);
			pin_pad.PlayVoice(PINPad::BuiltInVoice::kEnterPWD);
			pin_pad.SetPWDMaxLen(PWD_MAX_LEN);
			PINPad::StatusCode res = pin_pad.ReadUserInputPWD(PINPad::DispMode::kEncrypted, PINPad::DispLine::kL2, TIMEOUT, pwd);
			if (PINPad::StatusCode::kCancelled == res)
			{
				reply.error_code = StatusCode::PINPAD_CANCELLED;
				LOG_ERROR(L"用户取消密码输入。");
			}
			else if (PINPad::StatusCode::kTimeout == res)
			{
				reply.error_code = StatusCode::PINPAD_TIMEOUT;
				LOG_ERROR(L"用户输入密码超时。");
			}
			else if (PINPad::StatusCode::kUnknown == res)
			{
				reply.error_code = StatusCode::PINPAD_READ_FAILED;
				LOG_ERROR(L"读取用户输入密码失败，请确认密码键盘连接正常！");
			}
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"读取用户输入密码发生异常，异常信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::PINPAD_READ_FAILED;
			break;
		}
	} while (0);
	pin_pad.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"读取用户输入密码成功。");

	reply.data.put("password", pwd);
	return reply;
}

ReadPINPadCmd::Reply ReadIDCardCmd::Execute(const std::string& input)
{
	LOG_TRACE(L"读取身份证信息。");
	auto& id_card_reader = IDCardReader_();
	Reply reply;
	IDCardInfo id_card_info;
	do
	{
		try
		{
			if (!id_card_reader.Connect())
			{
				LOG_ERROR(L"建立身份证读卡器连接失败！");
				reply.error_code = StatusCode::ID_CARD_READER_CONNECT_FAILED;
				break;
			}
			if (!id_card_reader.VerifyCard())
			{
				LOG_ERROR(L"未找到身份证或身份证验证失败！");
				reply.error_code = StatusCode::ID_CARD_READER_VERIFY_FAILED;
				break;
			}
			id_card_info = id_card_reader.ReadCard();
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"读取身份证信息失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::ID_CARD_READER_READ_FAILED;
			break;
		}
	} while (0);
	id_card_reader.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"读取身份证信息成功。");

	reply.data.put("id", wstr_2_str(id_card_info.id_code));
	reply.data.put("name", wstr_2_str(id_card_info.name)); // Use 'wstr_2_str' to convert to UTF-8 string.
	reply.data.put("gender", wstr_2_str(id_card_info.gender));
	reply.data.put("nation", wstr_2_str(id_card_info.nation));
	reply.data.put("birthday", wstr_2_str(id_card_info.birth_date));
	reply.data.put("address", wstr_2_str(id_card_info.address));
	reply.data.put("portrait_img_path", wstr_2_str(id_card_info.portrait_img_path));
	reply.data.put("depart", wstr_2_str(id_card_info.depart));
	reply.data.put("start_date", wstr_2_str(id_card_info.start_date));
	reply.data.put("end_date", wstr_2_str(id_card_info.end_date));
	return reply;
}

ReadPINPadCmd::Reply ActivateDILICardCmd::Execute(const std::string& input)
{
	const uint8_t BEEP_TIME = 20; // 20 * 10ms
	const std::string PWD_MACHINE_IP = ApplicationConfig::Instance().PWDMachineIP();
	const uint32_t PWD_MACHINE_PORT = ApplicationConfig::Instance().PWDMachinePort();
	const uint8_t TIMEOUT = ApplicationConfig::Instance().PWDMachineTimeout();

	LOG_TRACE(L"卡片激活。");
	auto& dili_card_device = DILICardRWDevice();
	auto& pwd_machine = PWDMachine_();
	Reply reply;
	std::string card_sn;
	std::string key_DCCK, key_F0015_DACK, key_F0015_DEAK, key_F0015_DAMK1;
	DILICard::BasicInfo card_basic_info;
	do
	{
		if (!dili_card_device.Connect())
		{
			LOG_ERROR(L"打开CPU卡读卡器失败！");
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
			break;
		}
		if (!pwd_machine.Connect(PWD_MACHINE_IP, PWD_MACHINE_PORT))
		{
			LOG_ERROR(L"连接加密机失败！");
			reply.error_code = StatusCode::PWD_MACHINE_CONNECT_FAILED;
			break;
		}
		dili_card_device.Beep(BEEP_TIME);
		try { card_sn = dili_card_device.FindCard(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"查找卡片失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CANNOT_FIND_CARD;
			break;
		}
		try
		{
			key_DCCK = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kDCCK>(), card_sn, TIMEOUT);
			key_F0015_DACK = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DACK>(), card_sn, TIMEOUT);
			key_F0015_DEAK = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DEAK>(), card_sn, TIMEOUT);
			key_F0015_DAMK1 = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DAMK1>(), card_sn, TIMEOUT);
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"从加密机读取密钥失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::PWD_MACHINE_GET_KEY_FAILED;
			break;
		}

		try
		{
			dili_card_device.ActivateCard(card_sn, key_DCCK, key_F0015_DACK, key_F0015_DEAK, key_F0015_DAMK1);
			card_basic_info = dili_card_device.ReadCardBasicInfo();
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"卡片激活失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::DILI_CARD_DEVICE_ACTIVATE_CARD_FAILED;
			break;
		}
	} while (0);
	dili_card_device.Disconnect();
	pwd_machine.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"卡片激活成功。");

	reply.data.put("chipNo", card_basic_info.chip_num);
	reply.data.put("deviceId", card_basic_info.device_id);
	reply.data.put("type", card_basic_info.type_code);
	reply.data.put("issuerCode", card_basic_info.issuer_code);
	reply.data.put("verifyCode", card_basic_info.verify_code);
	return reply;
}

ReadPINPadCmd::Reply ResetDILICardCmd::Execute(const std::string& input)
{
	const uint8_t BEEP_TIME = 20; // 20 * 10ms
	const std::string PWD_MACHINE_IP = ApplicationConfig::Instance().PWDMachineIP();
	const uint32_t PWD_MACHINE_PORT = ApplicationConfig::Instance().PWDMachinePort();
	const uint8_t TIMEOUT = ApplicationConfig::Instance().PWDMachineTimeout();

	LOG_TRACE(L"卡片撤销激活。");
	auto& dili_card_device = DILICardRWDevice();
	auto& pwd_machine = PWDMachine_();
	Reply reply;
	std::string card_sn;
	std::string key_DCCK, key_F0015_DACK;
	DILICard::BasicInfo card_basic_info;
	do
	{
		if (!dili_card_device.Connect())
		{
			LOG_ERROR(L"打开CPU卡读卡器失败！");
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
			break;
		}
		if (!pwd_machine.Connect(PWD_MACHINE_IP, PWD_MACHINE_PORT))
		{
			LOG_ERROR(L"连接加密机失败！");
			reply.error_code = StatusCode::PWD_MACHINE_CONNECT_FAILED;
			break;
		}
		dili_card_device.Beep(BEEP_TIME);
		try { card_sn = dili_card_device.FindCard(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"查找卡片失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CANNOT_FIND_CARD;
			break;
		}
		try
		{
			key_DCCK = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kDCCK>(), card_sn, TIMEOUT);
			key_F0015_DACK = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DACK>(), card_sn, TIMEOUT);
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"从加密机读取密钥失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::PWD_MACHINE_GET_KEY_FAILED;
			break;
		}
		try
		{
			dili_card_device.ResetCard(key_DCCK, key_F0015_DACK);
			card_basic_info = dili_card_device.ReadCardBasicInfo();
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"卡片撤销激活失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::DILI_CARD_DEVICE_RESET_CARD_FAILED;
			break;
		}
	} while (0);
	dili_card_device.Disconnect();
	pwd_machine.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"卡片撤销激活成功。");

	reply.data.put("chipNo", card_basic_info.chip_num);
	reply.data.put("deviceId", card_basic_info.device_id);
	reply.data.put("type", card_basic_info.type_code);
	reply.data.put("issuerCode", card_basic_info.issuer_code);
	reply.data.put("verifyCode", card_basic_info.verify_code);
	return reply;
}

ReadPINPadCmd::Reply ReadDILICardBasicInfoCmd::Execute(const std::string& input)
{
	static const uint8_t BEEP_TIME = 20; // 20 * 10ms
	LOG_TRACE(L"读取卡片基本信息。");
	auto& dili_card_device = DILICardRWDevice();
	Reply reply;
	DILICard::BasicInfo card_basic_info;
	do
	{
		if (!dili_card_device.Connect())
		{
			LOG_ERROR(L"打开CPU卡读卡器失败！");
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
			break;
		}
		dili_card_device.Beep(BEEP_TIME);
		try { dili_card_device.FindCard(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"查找卡片失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CANNOT_FIND_CARD;
			break;
		}
		try { card_basic_info = dili_card_device.ReadCardBasicInfo(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"读取卡片基本信息失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::DILI_CARD_DEVICE_READ_BASIC_INFO_FAILED;
			break;
		}
	} while (0);
	dili_card_device.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"读取卡片基本信息成功。");

	reply.data.put("chipNo", card_basic_info.chip_num);
	reply.data.put("deviceId", card_basic_info.device_id);
	reply.data.put("type", card_basic_info.type_code);
	reply.data.put("issuerCode", card_basic_info.issuer_code);
	reply.data.put("verifyCode", card_basic_info.verify_code);
	return reply;
}

ReadPINPadCmd::Reply ReadDILICardServiceInfoCmd::Execute(const std::string& input)
{
	const std::string PWD_MACHINE_IP = ApplicationConfig::Instance().PWDMachineIP();
	const uint32_t PWD_MACHINE_PORT = ApplicationConfig::Instance().PWDMachinePort();
	const uint8_t TIMEOUT = ApplicationConfig::Instance().PWDMachineTimeout();
	const uint8_t BEEP_TIME = 20; // 20 * 10ms
	LOG_TRACE(L"读取卡片业务信息。");
	auto& dili_card_device = DILICardRWDevice();
	auto& pwd_machine = PWDMachine_();
	Reply reply;
	std::string card_sn;
	std::string key_r;
	std::string service_data;
	do
	{
		if (!dili_card_device.Connect())
		{
			LOG_ERROR(L"打开CPU卡读卡器失败！");
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
			break;
		}
		if (!pwd_machine.Connect(PWD_MACHINE_IP, PWD_MACHINE_PORT))
		{
			LOG_ERROR(L"连接加密机失败！");
			reply.error_code = StatusCode::PWD_MACHINE_CONNECT_FAILED;
			break;
		}
		dili_card_device.Beep(BEEP_TIME);
		try { card_sn = dili_card_device.FindCard(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"查找卡片失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CANNOT_FIND_CARD;
			break;
		}
		try { key_r = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DEAK>(), card_sn, TIMEOUT); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"从加密机读取密钥失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::PWD_MACHINE_GET_KEY_FAILED;
			break;
		}

		try { service_data = dili_card_device.ReadCardServiceInfo(key_r); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"读取卡片业务信息失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::DILI_CARD_DEVICE_READ_SERVICE_INFO_FAILED;
			break;
		}
	} while (0);
	dili_card_device.Disconnect();
	pwd_machine.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"读取卡片业务信息成功。");

	reply.data.put("", service_data);
	return reply;
}

ReadPINPadCmd::Reply WriteDILICardServiceInfoCmd::Execute(const std::string& input)
{
	const char DATA_SPLITER = ':';
	const uint8_t BEEP_TIME = 20; // 20 * 10ms
	const std::string PWD_MACHINE_IP = ApplicationConfig::Instance().PWDMachineIP();
	const uint32_t PWD_MACHINE_PORT = ApplicationConfig::Instance().PWDMachinePort();
	const uint8_t TIMEOUT = ApplicationConfig::Instance().PWDMachineTimeout();

	LOG_TRACE(L"写入卡片业务信息。");
	auto& dili_card_device = DILICardRWDevice();
	auto& pwd_machine = PWDMachine_();
	Reply reply;
	std::string card_sn;
	std::string key_w;
	do
	{
		std::size_t index = input.find(DATA_SPLITER);
		if (std::string::npos == index) // NOT found.
		{
			LOG_ERROR(L"写入卡片业务信息失败，错误信息：请求数据错误。");
			reply.error_code = StatusCode::DILI_CARD_DEVICE_WRITE_SERVICE_INFO_FAILED;
			break;
		}
		std::string device_id = input.substr(0, index);
		std::string service_data = input.substr(index + 1, input.size() - index - 1);
		if (!dili_card_device.Connect())
		{
			LOG_ERROR(L"打开CPU卡读卡器失败！");
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
			break;
		}
		dili_card_device.Beep(BEEP_TIME);
		try { card_sn = dili_card_device.FindCard(); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"查找卡片失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::CPU_CARD_DEVICE_CANNOT_FIND_CARD;
			break;
		}
		// 验证操作的是否是同一张卡，即device_id是否匹配
		if (!boost::iequals(card_sn, device_id))
		{
			LOG_ERROR(L"写卡卡片的序列号不匹配！");
			reply.error_code = StatusCode::DILI_CARD_DEVICE_WRITE_UNMATCHED_CARD;
			break;
		}
		if (!pwd_machine.Connect(PWD_MACHINE_IP, PWD_MACHINE_PORT))
		{
			LOG_ERROR(L"连接加密机失败！");
			reply.error_code = StatusCode::PWD_MACHINE_CONNECT_FAILED;
			break;
		}
		try { key_w = pwd_machine.GetCardPassword(DILICard::CardKeyCode<DILICard::KeyCode::kF0015_DAMK1>(), card_sn, TIMEOUT); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"从加密机读取密钥失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::PWD_MACHINE_GET_KEY_FAILED;
			break;
		}

		try { dili_card_device.WriteCardServiceInfo(key_w, service_data); }
		catch (std::exception& ex)
		{
			LOG_FATAL(L"写入卡片业务信息失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::DILI_CARD_DEVICE_WRITE_SERVICE_INFO_FAILED;
			break;
		}
	} while (0);
	dili_card_device.Disconnect();
	pwd_machine.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"写入卡片业务信息成功。");
	reply.data.put("", "");
	return reply;
}

ReadPINPadCmd::Reply ReadBankCardNumCmd::Execute(const std::string& input)
{
	LOG_TRACE(L"读取银行卡号。");
	auto& bank_card_reader = BankCardReader_();
	std::string bank_card_no("");
	Reply reply;
	do
	{
		try
		{
			if (!bank_card_reader.Connect())
			{
				LOG_ERROR(L"连接银行卡读卡器失败。");
				reply.error_code = StatusCode::CPU_CARD_DEVICE_CONNECT_FAILED;
				break;
			}
			bank_card_no = bank_card_reader.ReadBankCardNum();
		}
		catch (std::exception& ex)
		{
			LOG_FATAL(L"读取银行卡号失败，错误信息： " + str_2_wstr(ex.what()));
			reply.error_code = StatusCode::BANK_CARD_READ_CARD_NUM_FAILED;
			break;
		}
	} while (0);
	bank_card_reader.Disconnect();
	if (boost::iequals(reply.error_code, StatusCode::OK))
		LOG_TRACE(L"读取银行卡号成功。");

	reply.data.put("chipNo", bank_card_no);
	return reply;
}
