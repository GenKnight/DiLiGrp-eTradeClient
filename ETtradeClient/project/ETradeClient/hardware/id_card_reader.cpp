#include "stdafx.h"

#include <sstream>

#include <boost/filesystem.hpp>

#include "etradeclient/hardware/id_card_reader.h"
#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/string_converter.h"

namespace
{
	enum ReturnStatus /*Here we don't use strong typed enum to be compatible with the card reader library.*/
	{
		kOK = 1,
	};
}

const std::string IDCardReader::ID_CARD_READER_HARDWARE_INFO = "China-Vision_CVR-100U";

IDCardReader::IDCardReader()
{
	Init();
}
IDCardReader::~IDCardReader()
{
	Disconnect();
}

bool IDCardReader::Connect() const
{
	const int kUSBPortMin = 1001, kUSBPortMax = 1016; // Connect the card reader via USB.
	const int kCOMPortMin = 1, kCOMPortMax = 16; // Connect the card reader via COM.

	static const uint8_t RETRY_COUNT = 10;
	uint8_t conn_count = 0;
	do
	{
		if (DoConnect(kUSBPortMin, kUSBPortMax) || DoConnect(kCOMPortMin, kCOMPortMax))
			return true;
		++conn_count;
	} while (conn_count < RETRY_COUNT);
	return false;
}

void IDCardReader::Disconnect() const
{
	m_close_comm_fn();
}

bool IDCardReader::VerifyCard() const
{
	// First, do the authentication between the ID card and the card reader, so make sure the 
	// ID card is placed on top of the card reader when this funciton is called.
	return kOK == m_auth_fn();
}

IDCardInfo IDCardReader::ReadCard() const
{
	// Read card content, the param '1' is just for commpatibility reason, no practical meaning.
	auto res = m_read_content_fn(1);
	if (kOK != res)
	{
		std::stringstream err_msg;
		err_msg << "Read ID card content failed, error code: " << res;
		throw std::exception(err_msg.str().c_str());
	}
	// Get every field of the card info.
	IDCardInfo id_info;
	GetCardField(m_get_name_fn, id_info.name);
	GetCardField(m_get_gender_fn, id_info.gender);
	GetCardField(m_get_nation_fn, id_info.nation);
	GetCardField(m_get_birthday_fn, id_info.birth_date);
	GetCardField(m_get_addr_fn, id_info.address);
	GetCardField(m_get_id_code_fn, id_info.id_code);
	GetCardField(m_get_dept_fn, id_info.depart);
	GetCardField(m_get_start_date_fn, id_info.start_date);
	GetCardField(m_get_end_data_fn, id_info.end_date);
	GetPortraitPath(id_info.portrait_img_path);
	return id_info;
}

void IDCardReader::Init() // Load library to initialize all the function object.
{
	const std::wstring kDLLFile = L"./termb.dll";
	HINSTANCE dll_handle = LoadLibrary(kDLLFile.c_str());
	if (NULL != dll_handle)
	{
		m_init_comm_fn = (InitCommFunc)GetProcAddress(dll_handle, "CVR_InitComm");
		m_close_comm_fn = (CloseCommFunc)GetProcAddress(dll_handle, "CVR_CloseComm");
		m_auth_fn = (AuthenticateFunc)GetProcAddress(dll_handle, "CVR_Authenticate");
		m_read_content_fn = (ReadContentFunc)GetProcAddress(dll_handle, "CVR_Read_Content");
		m_get_name_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleName");
		m_get_gender_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleSex");
		m_get_nation_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleNation");
		m_get_birthday_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleBirthday");
		m_get_addr_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleAddress");
		m_get_id_code_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetPeopleIDCode");
		m_get_dept_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetDepartment");
		m_get_start_date_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetStartDate");
		m_get_end_data_fn = (ReadContentFieldFunc)GetProcAddress(dll_handle, "GetEndDate");
	}
	else // If APIs are not successfully loaded, Log and then throw exception.
		throw std::exception(("Fail to load the ID card reader DLL from path : " + wstr_2_str(kDLLFile)).c_str());
}

bool IDCardReader::DoConnect(int port_min, int port_max) const
{
	for (int port = port_min; port <= port_max; ++port)
	{
		auto res = m_init_comm_fn(port);
		if (kOK == res)
			return true;
	}
	return false;
}

void IDCardReader::GetCardField(ReadContentFieldFunc fn, std::wstring& field) const
{
	const int kFieldMaxLen = 128;
	char field_[kFieldMaxLen] = {'\0'};
	int len = 0;

	if (kOK == fn(field_, &len))
		field = gbk_2_wstr(field_); // The library will read filed into bytes in GBK encoding, need to convert to Unicode string.
	else
		throw std::exception("Calling ID card reader API to read ID card field failed.");
}

void IDCardReader::GetPortraitPath(std::wstring& portrait_path) const
{
	namespace fs = boost::filesystem;
	const std::string kPortraitName("zp.bmp");

	wchar_t exe_path[MAX_PATH];
	HMODULE h_module = GetModuleHandle(NULL);
	if (NULL != h_module)
		GetModuleFileName(h_module, exe_path, sizeof(exe_path));
	else
		throw std::exception("Calling windows API to get exe path failed.");

	// @TODO modify the path?
	std::wstring portrait_path_ = fs::wpath(exe_path).parent_path().wstring() + str_2_wstr("\\" + kPortraitName);
	if (!fs::exists(portrait_path_))
		throw std::exception("ID card portrait image does not exist.");

	portrait_path = portrait_path_;
}