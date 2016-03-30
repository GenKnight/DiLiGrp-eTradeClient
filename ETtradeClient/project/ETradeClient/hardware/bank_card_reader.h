#ifndef ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED
#define ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED

/*Read & write device for the bank card.
The APIs may throw exceptions when error happen. The caller side need to handle the exceptions.*/

/*
CPU卡读写器产品信息：
生产厂商：深圳市德卡科技有限公司
型号：D3
*/

#include "windows.h"

#include <string>

class BankCardReader
{
	typedef std::basic_string<unsigned char> RecordDataT;
public:
	BankCardReader();
	~BankCardReader();
	bool Connect() const;
	void Disconnect() const;
	std::string ReadBankCardNum() const;

private:
	void FindAndResetCard() const ;
	void SelectDF() const;
	// 底层接口放回的数据为 unsigned char，所以此处用std::basic_string<unsigned char>存放数据并做返回值
	RecordDataT ReadDFRecord() const;
	std::string ParseDFRecord(const RecordDataT& data) const;

private:
	mutable HANDLE m_icdev;
};
#endif // ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED
