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
密码键盘产品信息：
生产厂商：深圳市德卡科技有限公司
型号：P3AS000UB

内置显示文字之ID与文字对应关系：
1->请输入密码
2->请再次输入密码

语音提示之ID与语音对应关系：
1->请输入密码
2->请输入原密码
3->请再输入一次
4->请输入新密码
5->请确认新密码
6->密码修改成功
7->请插卡
8->请刷卡
9->密码错误
*/


class PINPad
{
public:
	/*内置显示内容*/
	enum class BuiltInText
	{
		kEnterPWD = 1, // "请输入密码"
		kReEnterPWD // "请再次输入密码"
	};

	/*内置语音输出*/
	enum class BuiltInVoice
	{
		kEnterPWD = 1,		// "请输入密码"
		kEnterOriginalPWD,	// "请输入原密码"
		kReEnterPWD,		// "请再输入一次"
		kEnterNewPWD,		// "请输入新密码"
		kConfirmNewPWD,		// "请确认新密码"
		kChangePWDSucceed,	// "密码修改成功"
		kInsertCard,		// "请插卡"
		kSwipeCard,			// "请刷卡"
		kPWDIncorrect		// "密码错误"
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
