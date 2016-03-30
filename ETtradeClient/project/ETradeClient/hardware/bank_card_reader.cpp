#include "stdafx.h"
#include "etradeclient/hardware/bank_card_reader.h"

#include "etradeclient/hardware/ex_reference/bank_card_reader/dcrf32.h"

#include <exception>
#include <cstdint>
#include <sstream>
#include <iomanip> // hex conversion.
#include <intrin.h> // Endian conversion.

template <typename T>
std::string DecToHex(T d, bool show_base = false)
{
	std::stringstream ss;
	if (show_base)
		ss << "0x";
	ss << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << static_cast<int>(d);
	return ss.str();
}

BankCardReader::BankCardReader() : m_icdev(NULL)
{}

BankCardReader::~BankCardReader()
{
	Disconnect();
}

bool BankCardReader::Connect() const
{
	static const uint8_t RETRY_COUNT = 10;
	
	const int kUSBPort = 100, kBaudRate = 0; // port 为 100时，表示使用USB接口通讯，则波特率无效，因此设置为0.
	const int kBeepMs = 10;
	uint8_t conn_count = 0;
	do
	{
		m_icdev = dc_init(kUSBPort, kBaudRate);
		if (0 < (int)m_icdev)
		{
			dc_beep(m_icdev, kBeepMs);
			return true;
		}
		++conn_count;
	} while (conn_count < RETRY_COUNT);
	return false;
}
 
void BankCardReader::Disconnect() const
{
	dc_exit(m_icdev);
}

std::string BankCardReader::ReadBankCardNum() const
{
	FindAndResetCard(); // 读卡之前必须要先寻卡并复位卡，否则会读取失败。
	SelectDF();
	return ParseDFRecord(ReadDFRecord());
}

void BankCardReader::FindAndResetCard() const // 寻卡并复位卡片
{
	unsigned long snr = 0;
	unsigned char lenth[128] = {0};
	unsigned char data[128] = {0};
	int st = 0;

	st = dc_card(m_icdev, 0, &snr); // 一定要先寻卡
	if (0 != st)
		throw std::exception("Find bank card failed!");
	st = dc_pro_reset(m_icdev, lenth, data);
	if (0 != st)
		throw std::exception("Reset bank card failed!");
}

// Select 选择文件DF
void BankCardReader::SelectDF() const
{
	const int kOK = 0;
	const unsigned int kTimeout = 7; // 延迟时间，单位为：10ms, 7 来自与官方示例，该值如果设过大会导致操作失败.
	const unsigned int kMaxDataSize = 100;
	const unsigned char kSendDataLen = 13; // 发送的信息长度
	unsigned char recv_data_len = 0; // 读取信息的长度
	unsigned char send_data[kMaxDataSize] = { 0 }, recv_data[kMaxDataSize] = { 0 };

	/*APDU指令：【选择文件或应用】，参考PBOC2.0标准.
	* 指令格式：[CLA][INS][P1][P2][Lc][Data] ,指令值：[00][A4][04][00][08][A000000333010101]

	* 代码 值 
	* CLA "00" 
	* INS "A4" 
	* P1  "00"：按FID选择（P2必须等于00）; "01"：按FID选择DF; "02"：按FID选择EF; "04"：按文件名选择DF 
	* P2  "00"第一个或仅有一个， "02"选择下一个（按文件名选择）， 
	* Lc  当P1="00"时，该值设定为"02"; 当P1="04"，该值设定为Data的长度，范围为"05"-"10"， 
	* Data 文件标识符或DF文件名 
	*/
	send_data[0] = 0x00; // CLA
	send_data[1] = 0xA4; // INS
	send_data[2] = 0x04; // P1，按文件名选择DF
	send_data[3] = 0x00; // P2
	send_data[4] = 0x08; // Lc 数据长度
	// Data：DF文件名“A000000333010101”
	send_data[5] = 0xA0; send_data[6] = 0x00;  send_data[7] = 0x00;  send_data[8] = 0x03;
	send_data[9] = 0x33; send_data[10] = 0x01; send_data[11] = 0x01; send_data[12] = 0x01;

	if (kOK != dc_pro_command(m_icdev, kSendDataLen, send_data, &recv_data_len, recv_data, kTimeout))
		throw std::exception("Select DF failed!");
}

// Read Record 读DF记录文件
BankCardReader::RecordDataT BankCardReader::ReadDFRecord() const
{
	const int kOK = 0;
	const unsigned int kTimeout = 7; // 延迟时间，单位为：10ms, 7 来自与官方示例，该值如果设过大会导致操作失败.
	const unsigned int kMaxDataSize = 100;
	const unsigned char kSendDataLen = 5; // 发送的信息长度
	unsigned char recv_data_len = 0; // 读取信息的长度
	unsigned char send_data[kMaxDataSize] = { 0 }, recv_data[kMaxDataSize] = { 0 };

	/*APDU指令：【读记录】，参考PBOC2.0标准
	* 指令格式：[CLA][INS][P1][P2][Le] ,指令值：[00][B2][01][14][00]

	* 代码	值 
	* CLA 	"00"
	* INS 	"B2" 
	* P1	记录号, "01"
	* P2	引用控制参数 
	* Lc 	不存在； 
	* Data 	不存在； 
	* Le 	"00"或者要读取的长度*/
	send_data[0] = 0x00; // CLA
	send_data[1] = 0xB2; // INS
	send_data[2] = 0x01; // P1
	send_data[3] = 0x14; // P2，按文件记录号访问
	send_data[4] = 0x00; // Le
	if (kOK != dc_pro_command(m_icdev, kSendDataLen, send_data, &recv_data_len, recv_data, kTimeout))
		throw std::exception("Read DF record failed!");
	return RecordDataT(recv_data, recv_data_len);// 赋值的时候，指明长度，避免处理过程中遇到'\0'就结束
}

std::string BankCardReader::ParseDFRecord(const RecordDataT& data) const
{
	//数据格式：[卡号标签][卡号长度][卡号][填充字符]
	//如：5A0A6230910299000378541f --- [5A][0A][6230910299000378541][f].
	const unsigned char kCardNumTag = 0x5A;// 银行卡号存放位置指示标签
	unsigned char card_num_length = 0;

	std::size_t pos = data.find_first_of(kCardNumTag);
	if (pos != std::string::npos) // 找到标签
		card_num_length = data.at(++pos);// 读取卡号长度
	else
		throw std::exception("Read bank card num failed!");
	
	std::string card_num;
	++pos;
	for (int i = 0; i < card_num_length; ++i, ++pos)
		card_num.append(DecToHex(data.at(pos)));
	
	// 如果银行卡号是奇数位，最后一位会用“f”补齐，因此读出数据的最后一位需要排除。
	const int kPaddingCharCount = 1;
	int card_last_num_pos = card_num_length * 2 - kPaddingCharCount;
	const char kPaddingChar = 'f';// 卡号填充字符
	if (kPaddingChar == card_num.at(card_last_num_pos))
		card_num.erase(card_last_num_pos, kPaddingCharCount);
	return card_num;

}