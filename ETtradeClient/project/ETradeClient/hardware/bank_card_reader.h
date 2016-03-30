#ifndef ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED
#define ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED

/*Read & write device for the bank card.
The APIs may throw exceptions when error happen. The caller side need to handle the exceptions.*/

/*
CPU����д����Ʒ��Ϣ��
�������̣������е¿��Ƽ����޹�˾
�ͺţ�D3
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
	// �ײ�ӿڷŻص�����Ϊ unsigned char�����Դ˴���std::basic_string<unsigned char>������ݲ�������ֵ
	RecordDataT ReadDFRecord() const;
	std::string ParseDFRecord(const RecordDataT& data) const;

private:
	mutable HANDLE m_icdev;
};
#endif // ETRADECLIENT_HARDWARE_BANK_CARD_READER_H_INCLUDED
