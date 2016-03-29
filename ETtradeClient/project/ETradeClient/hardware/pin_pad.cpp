#include "stdafx.h"

#include "etradeclient/hardware/pin_pad.h"

#include "etradeclient/hardware/ex_reference/pin_pad/dcp3.h"

static const uint8_t PWD_LEN_MAX = 16;

// "dcp3" library's error code when get password.
static const int16_t GET_PWD_SUCCESS = 0x0;
static const int16_t GET_PWD_ERROR_CANCELLED = -0xa1;
static const int16_t GET_PWD_ERROR_TIMEOUT = -0xa2;

PINPad::~PINPad()
{
	Disconnect();
}

bool PINPad::Connect() const
{
	static const uint8_t RETRY_COUNT = 10;
	uint8_t conn_count = 0;
	do
	{
		if (DoConnect())
			return true;
		++conn_count;
	} while (conn_count < RETRY_COUNT);
	return false;
}

void PINPad::Disconnect() const
{
	if ((int32_t)m_handle > 0)
		P3_IC_ExitComm(m_handle);
}

bool PINPad::DisplayBuiltInText(const BuiltInText& text) const
{
	return 0 == P3_IC_DispLcd(m_handle, static_cast<uint8_t>(text));
}

bool PINPad::PlayVoice(const BuiltInVoice& voice) const
{
	return 0 == P3_IC_PlayVoice(m_handle, static_cast<uint8_t>(voice));
}

bool PINPad::SetPWDMaxLen(uint8_t len) const
{
	const uint8_t kPWDLenMin = 1;

	if (len < kPWDLenMin)
		len = kPWDLenMin;
	else if (len > PWD_LEN_MAX)
		len = PWD_LEN_MAX;
	return 0 == P3_IC_SetInPutKeyLength(m_handle, len);
}

bool PINPad::SwitchLCDLight(bool on_off) const
{
	uint8_t flag = on_off ? 1 : 0;
	return 0 == P3_IC_CtlBackLight(m_handle, flag);
}

PINPad::StatusCode PINPad::ReadUserInputPWD(const DispMode& disp_mode, const DispLine& disp_line, uint8_t timeout, std::string& pwd) const
{
	return DoReadUserInputPWD(std::bind(P3_IC_GetInputKey, m_handle, static_cast<uint8_t>(disp_mode), static_cast<uint8_t>(disp_line),
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), timeout, pwd);
}

bool PINPad::ClearScreen() const
{
	const uint8_t kTotalLineNum = 2;
	return 0 == P3_IC_LcdClrScrn(m_handle, kTotalLineNum);
}

std::string PINPad::GetVersion() const
{
	const int kVersionMaxLen = 200;
	unsigned char version[kVersionMaxLen] = { '\0' };
	P3_IC_ReadVer(m_handle, version);
	return reinterpret_cast<char*>(version);
}

bool PINPad::DoConnect() const
{
	const int16_t kCOMPortMin = 0, kCOMPortMax = 19, kUSBPort = 100;
	const uint32_t kBaudRate = 19200;
	do
	{
		// First try connceting with USB port.
		m_handle = P3_IC_InitComm_Baud(kUSBPort, kBaudRate);
		if ((int32_t)m_handle > 0)
			return true; // Connection established.
		for (uint8_t com_port = kCOMPortMin; com_port <= kCOMPortMax; ++com_port)
		{
			m_handle = P3_IC_InitComm_Baud(com_port, kBaudRate);
			if ((int32_t)m_handle > 0)
				return true; // Connection established.
		}
	} while (0);
	return false;
}

PINPad::StatusCode PINPad::DoReadUserInputPWD(ReadUserInputPWDFn fn, uint8_t timeout, std::string& pwd) const
{
	uint8_t PWD_len = 0;
	unsigned char PWD[PWD_LEN_MAX] = { '\0' };
	int16_t status = fn(timeout, &PWD_len, PWD);
	switch (status)
	{
	case GET_PWD_SUCCESS:
		pwd = reinterpret_cast<char*>(PWD);
		return StatusCode::kOK;
	case GET_PWD_ERROR_CANCELLED:
		return StatusCode::kCancelled;
	case GET_PWD_ERROR_TIMEOUT:
		return StatusCode::kTimeout;
	default:
		return StatusCode::kUnknown;
	}
}