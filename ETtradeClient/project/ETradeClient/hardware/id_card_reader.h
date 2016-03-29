#ifndef ETRADECLIENT_HARDWARE_ID_CARD_READER_H_INCLUDED
#define ETRADECLIENT_HARDWARE_ID_CARD_READER_H_INCLUDED

/*
密码机信息：
生产厂商：广州江南科友科技股份有限公司
型号：（未知）
*/

#include <string>

struct  IDCardInfo
{
	std::wstring name;
	std::wstring gender;
	std::wstring nation;
	std::wstring birth_date;
	std::wstring address;
	std::wstring id_code;
	std::wstring depart;
	std::wstring start_date;
	std::wstring end_date;
	std::wstring portrait_img_path;
};

class IDCardReader
{
	typedef int(__stdcall *InitCommFunc)(int port);
	typedef int(__stdcall *CloseCommFunc)();
	typedef int(__stdcall *AuthenticateFunc)();
	typedef int(__stdcall *ReadContentFunc)(int active);

	typedef int(__stdcall *ReadContentFieldFunc)(char* name, int* len);

public:
	/*This function may throw exception if the card reader library cannot be loaded!*/
	IDCardReader();
	~IDCardReader();
	bool Connect() const;
	void Disconnect() const;
	bool VerifyCard() const;
	IDCardInfo ReadCard() const;
	
private:
	void Init(); // Load library to initialize all the function object.
	bool DoConnect(int port_min, int port_max) const;
	void GetCardField(ReadContentFieldFunc fn, std::wstring& field) const;
	void GetPortraitPath(std::wstring& portrait_path) const;

	InitCommFunc		m_init_comm_fn; // Function object to init the communitation with ID card reader.
	CloseCommFunc		m_close_comm_fn; // Function object to close the communitation with ID card reader.
	AuthenticateFunc	m_auth_fn; // Function object to authentication of the ID card reader.
	ReadContentFunc		m_read_content_fn; // Function object to read the content from the ID card reader.
		
	ReadContentFieldFunc m_get_name_fn;
	ReadContentFieldFunc m_get_gender_fn;
	ReadContentFieldFunc m_get_nation_fn;
	ReadContentFieldFunc m_get_birthday_fn;
	ReadContentFieldFunc m_get_addr_fn;
	ReadContentFieldFunc m_get_id_code_fn;
	ReadContentFieldFunc m_get_dept_fn;
	ReadContentFieldFunc m_get_start_date_fn;
	ReadContentFieldFunc m_get_end_data_fn;

	static const std::string ID_CARD_READER_HARDWARE_INFO; // Currently this field is not used.
};
#endif // ETRADECLIENT_HARDWARE_ID_CARD_READER_H_INCLUDED
