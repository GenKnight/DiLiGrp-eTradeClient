#ifndef ETRADECLIENT_HARDWARE_DILI_CARD_DEVICE_H_INCLUDED
#define ETRADECLIENT_HARDWARE_DILI_CARD_DEVICE_H_INCLUDED

/*Read & write device for the DILI card.
The APIs may throw exceptions when error happen. The caller side need to handle the exceptions.*/

/*
CPU����д����Ʒ��Ϣ��
�������̣������е¿��Ƽ����޹�˾
�ͺţ�D3
*/

#include <string>
#include <cstdint>

namespace DILICard
{
	struct BasicInfo
	{
		std::string chip_num; // �����
		std::string device_id; // ��ƬӲ����ʶ����Ƭ����, �����к�SN��
		std::string type_code; // ��Ƭ�����루�����ͱ��룩
		std::string issuer_code; // ������������
		std::string verify_code; // У���루��ȫ�����(3λ��)��
	};

	// ����Կ����
	enum class KeyCode
	{
		kDCCK, // Card Mast KEY, ����MF�ṹ��Ӻ�ɾ��
		kF0015_DACK, // Ӧ����������Կ, ����Ӧ�����ڽṹ��Ӻ�ɾ��
		kF0015_DEAK, // �ⲿ��֤��Կ, ����0015�ļ��Ķ�Ȩ�޿���
		kF0015_DAMK1, // Ӧ��ά������Կ1, ����0015�ļ��ĸ��±���
	};

	template <KeyCode> inline std::string CardKeyCode();
	template <> inline std::string CardKeyCode<KeyCode::kDCCK>() { return "0000"; }
	template <> inline std::string CardKeyCode<KeyCode::kF0015_DACK>() { return "0000"; }
	template <> inline std::string CardKeyCode<KeyCode::kF0015_DEAK>() { return "0001"; }
	template <> inline std::string CardKeyCode<KeyCode::kF0015_DAMK1>() { return "0002"; }

	class RWDevice
	{
		typedef bool(__stdcall *OpenFn)();
		typedef void(__stdcall *CloseFn)();
		typedef void(__stdcall *BeepFn)(int time); // Unit: 10ms.
		typedef int(__stdcall *FindCardFn)(char* sn); // This function will read the card serial number.
		typedef bool(__stdcall *WriteBinaryDataFn)(char file_id, int offset, int length, const char* key, const char* data);
		typedef bool(__stdcall *ChangeDirFn)(const char* dir);
		typedef void(__stdcall *GetValueFn)(char* value);
		typedef bool(__stdcall *ReadBinaryDataFn)(char file_id, int offset, int length);
		typedef bool(__stdcall *ExternalAuthFn)(char key_id, const char* key);
		typedef bool(__stdcall *InstallKeyFn)(const char* main_key, const char* cmd_head, const char* install_key, bool is_main_key, bool);
	public:
		RWDevice();
		~RWDevice();

		bool Connect() const;
		void Disconnect() const;
		void Beep(uint32_t time) const; // Unit: 10ms.
		std::string FindCard() const; // If card found, card's serial number will be returned.
		void ActivateCard(const std::string& card_sn, 
						  const std::string& key_DCCK, 
						  const std::string& key_F0015_DACK,
						  const std::string& key_F0015_DEAK,
						  const std::string& key_F0015_DAMK1) const;
		void ResetCard(const std::string& key_DCCK, const std::string& key_F0015_DACK) const;
		BasicInfo ReadCardBasicInfo() const;
		std::string ReadCardServiceInfo(const std::string& key_F0015_DEAK) const;
		void WriteCardServiceInfo(const std::string& key_F0015_DAMK1, const std::string& data) const;
	private:
		void Init();
		std::string GetValue() const;
		std::string ReadCardFile(const std::string& dir, 
								 char				file_id, 
								 uint32_t			offset, 
								 uint32_t			length, 
								 char				key_id, 
								 const std::string& key) const;
		void WriteCardFile(const std::string&	dir, 
						   char					file_id, 
						   uint32_t				offset,
						   char					key_id, 
						   const std::string&	key, 
						   const std::string&	data) const;
		void SetPassword(const std::string& dir, 
						 char				key_id, 
						 const std::string& main_key, 
						 const std::string& cmd_head, 
						 const std::string& install_key, 
						 bool				is_main_key) const;
		
	private:
		OpenFn				m_fn_open;
		BeepFn				m_fn_beep;
		CloseFn				m_fn_close;
		FindCardFn			m_fn_find_card;
		WriteBinaryDataFn	m_fn_write_bin_data;
		ChangeDirFn			m_fn_ch_dir;
		GetValueFn			m_fn_get_value;
		ReadBinaryDataFn	m_fn_read_bin_data;
		ExternalAuthFn		m_fn_ex_auth;
		InstallKeyFn		m_fn_install_key;
	};
} // DILICard
#endif // ETRADECLIENT_HARDWARE_DILI_CARD_DEVICE_H_INCLUDED
