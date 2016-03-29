#ifndef ETRADECLIENT_HARDWARE_PIN_PAD_H_INCLUDED
#define ETRADECLIENT_HARDWARE_PIN_PAD_H_INCLUDED

#include <cstdint>
#include <string>
#include <functional>

/*A PIN pad or PIN entry device is an electronic device used in a debit, 
credit or smart card-based transaction to accept and encrypt the cardholder's 
personal identification number (PIN).
*/

/*
������̲�Ʒ��Ϣ��
�������̣������е¿��Ƽ����޹�˾
�ͺţ�P3AS000UB

������ʾ����֮ID�����ֶ�Ӧ��ϵ��
1->����������
2->���ٴ���������

������ʾ֮ID��������Ӧ��ϵ��
1->����������
2->������ԭ����
3->��������һ��
4->������������
5->��ȷ��������
6->�����޸ĳɹ�
7->��忨
8->��ˢ��
9->�������
*/


class PINPad
{
public:
	/*������ʾ����*/
	enum class BuiltInText
	{
		kEnterPWD = 1, // "����������"
		kReEnterPWD // "���ٴ���������"
	};

	/*�����������*/
	enum class BuiltInVoice
	{
		kEnterPWD = 1,		// "����������"
		kEnterOriginalPWD,	// "������ԭ����"
		kReEnterPWD,		// "��������һ��"
		kEnterNewPWD,		// "������������"
		kConfirmNewPWD,		// "��ȷ��������"
		kChangePWDSucceed,	// "�����޸ĳɹ�"
		kInsertCard,		// "��忨"
		kSwipeCard,			// "��ˢ��"
		kPWDIncorrect		// "�������"
	};

	/*Display mode on PIN pad's LCD of used inuput password.*/
	enum class DispMode
	{
		kNone,		// No display.
		kEncrypted, // Display the encrypted password in form of '*'.
		kPlain,		// Display plain text on LCD.
	};

	enum class DispLine
	{
		kL1 = 1,	// Display text on Line one.
		kL2			// Display text on Line two.
	};

	enum class StatusCode
	{
		kOK = 0,		// No error.
		kTimeout = 1,	// Timeout when get password.
		kCancelled = 2, // User input cancelled.
		kUnknown = 3	// Other unknown error.
	};

	typedef void* HANDLE;
public:
	~PINPad();
	bool Connect() const;
	void Disconnect() const;
	bool DisplayBuiltInText(const BuiltInText& text) const; // Display the PIN pad's built in text.
	bool PlayVoice(const BuiltInVoice& voice) const;
	bool SetPWDMaxLen(uint8_t len) const; // Max length should be between 1 & 16, why 16? I chose it because the manufacturer didn't provide this specification.
	bool SwitchLCDLight(bool on_off) const; // Set 'true' to turn on the light and 'false' to turn it off.
	StatusCode ReadUserInputPWD(const DispMode& disp_mode, const DispLine& disp_line, uint8_t timeout, std::string& pwd) const; // Timeout in seconds, minimum value is 1.
	bool ClearScreen() const;
	std::string GetVersion() const;
	
private:
	typedef std::function<short(unsigned char, unsigned char*, unsigned char*)> ReadUserInputPWDFn;

	bool DoConnect() const;
	StatusCode DoReadUserInputPWD(ReadUserInputPWDFn fn, uint8_t timeout, std::string& pwd) const;
	
	mutable HANDLE m_handle; // The hardware connection handle.
};

#endif // ETRADECLIENT_HARDWARE_PIN_PAD_H_INCLUDED
