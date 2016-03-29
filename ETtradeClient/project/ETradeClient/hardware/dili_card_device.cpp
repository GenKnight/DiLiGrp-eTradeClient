#include "stdafx.h"

#include <windows.h>

#include "etradeclient/hardware/dili_card_device.h"

#include <exception>
#include <sstream>
#include <iomanip> // hex conversion.
#include <intrin.h> // Endian conversion.

#include "etradeclient/utility/string_converter.h"

template <typename T>
std::string DecToHex(T d, bool show_base = false)
{
	std::stringstream ss;
	if (show_base)
		ss << "0x";
	ss << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << d;
	return ss.str();
}

/*
Note:
All data are stored in the card in binary format.
However, the data will be converted into hexadecimal character format when we read it via the card reader API.
In binary format, one character's length is 1 byte, 
and 1 character can be represented using HEX format which needs 2 characters(thus the length becomes 2).
For example, decimal:255 ---> binary:1111 1111 (length: 1 byte)---> hexadecimal: 0xFF (length: 2 bytes---'F''F').
This is the reason why the length of data read from card are always 2 times of the length of it's inner storage format(binary).
*/

namespace DILICard
{
	// ע�⣺�������ֵ�ĺ�5λ��Ϊ�����ͱ�Ƿ�Χ��
	struct CardTypeCfg
	{
		std::string code;
		uint32_t mark_num_min; // �����ͱ�Ƿ�Χ��Сֵ
		uint32_t mark_num_max; // �����ͱ�Ƿ�Χ���ֵ
	};

	// ע�⣺ǰ���ƿ������н���һ�����ķ�����������Ϊ8000���Ժ����еĿ��ķ������������Ϊ8888��
	static const std::string CARD_ISSUER_CODE_HAERBIN_HADA_BATCH1 = "8000"; // TODO Config file.
	static const std::string CARD_ISSUER_CODE_HAERBIN_HADA = "8888"; // �����������г�. TODO Config file.

	// ��ƬӲ����ʶĬ��ֵΪ��FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF������Ҫ�ڼ���ʱд����ȷֵ��ͨ������������FindCard����ã���
	// �������������ΪĬ��ֵ��
	static const std::string DEFAULT_CARD_DEVICE_ID = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";

	static const uint16_t DEFAULT_DATA_BLOCK_LEN = 512;
	static const uint16_t CARD_DATA_LEN_MAX = 4 * 1024;
	static const uint8_t WRITE_DATA_BLOCK_LEN = 32; // ����д�뵥Ԫ��С.
	static const uint16_t SERVICE_INFO_DATA_HEADER_LEN = 4; // ҵ���ļ�(FID:0015)ǰ4�ֽ�ȷ���ļ�����Ч����
	static const uint16_t SERVICE_INFO_DATA_LEN_MAX = 0xFA; // ҵ��������󳤶�250bytes.

	// ��������Ϣ�ļ�����������ţ���ƬӲ����ʶ����Ƭ����, �����к�SN������Ƭ�����룬�����������룬У���롣
	/*
	�ֽ�			����Ԫ				���ݸ�ʽ		���ȣ�Byte��
	1 - 6		�����				HEX			6
	7 - 14		�����ͱ���			BCD			8
	15 - 24		������������			BCD			10
	25 - 28		��ȫ�����(3λ��)	BCD			4
	29 - 45		��Ƭ����				BCD			16
	46 - 250	RFU					HEX			206
	*/

	/*Notice: All these length are the reserved maximum length, currently some of the fields' actual length are less than it's
	reserved length. E.g. verify code's actual length is 3 while it's reserved length is 8.
	The spare characters are all filled with character 'F'.*/
	static const uint8_t CARD_CHIP_NUM_LEN = 12; // �����
	static const uint8_t CARD_TYPE_CODE_LEN = 16; // ��Ƭ�����루�����ͱ��룩
	static const uint8_t CARD_ISSUER_CODE_LEN = 20; // ������������
	static const uint8_t CARD_VERIFY_CODE_LEN = 8; // У���루��ȫ�����(3λ��)��
	static const uint8_t CARD_SN_LEN = 32; // ��Ƭ����, �����к�SN

	static const char PADDING_CHAR = 'F';

	static const char* MF_FILE_DIR = "3F00";
	static const char* DF01_DIR = "DF01";

	static const char BASIC_INFO_FILE_ID = 0x5;
	static const char SERVICE_INFO_FILE_ID = 0x15;

	static const char SERVICE_INFO_KEY_ID = 0x1;
	static const char ACTIVATE_KEY_ID = 0x0;


	// ��ƬĬ����Կ
	static const char* K_DCCK_DV = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0";
	static const char* K_F0015_DACK_DV = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6";
	static const char* K_F0015_DEAK_DV = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7";
	static const char* K_F0015_DAMK1_DV = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8";

	static const char* K_F0005_DCMK1 = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4";

	static const char* K_DCCK_CMD_HEAD = "18000000003300FF10";
	static const char* K_F0015_DACK_CMD_HEAD = "18000000006600FF10";
	static const char* K_F0015_DEAK_CMD_HEAD = "180D01000066002210";
	static const char* K_F0015_DAMK1_CMD_HEAD = "184801000066002210";

	RWDevice::RWDevice()
	{
		Init();
	}
	RWDevice::~RWDevice()
	{
		Disconnect();
	}
	bool RWDevice::Connect() const
	{
		static const uint8_t RETRY_COUNT = 10;
		uint8_t conn_count = 0;
		do
		{
			if (m_fn_open())
				return true;
			++conn_count;
		} while (conn_count < RETRY_COUNT);
		return false;
	}
	void RWDevice::Disconnect() const
	{
		m_fn_close();
	}
	void RWDevice::Beep(uint32_t time) const
	{
		m_fn_beep(time);
	}
	std::string RWDevice::FindCard() const
	{
		static const uint8_t OK = 0;
		char sn[CARD_SN_LEN + 1] = { 0x0 };
		// ������顮m_fn_find_card���ķ���ֵ��������Ϊ�ú�����BUG����ʹδ����ȷ���������кţ�
		// �ú�����Ȼ���ܷ���0. ������ÿ����ȷ������Ƭ������ѿ����߲������ٴζ�������ʱ�ú���
		// �᷵��0������û�ж��������кš�
		if (OK == m_fn_find_card(sn) && sn[0] != 0x0)
			return sn;
		else
			throw std::exception("Card not found!");
	}

	// ��������Ϣ�ļ�����������ţ���ƬӲ����ʶ����Ƭ����, �����к�SN������Ƭ�����룬�����������룬У���롣
	// ��������£����еĿ�Ƭ�ڳ������䡰��ƬӲ����ʶ������ ��Ƭ�����롱���������������롱��Ӧ���Ѿ����úã�������Ҫ�ڼ����������д�롣
	// ��ˣ�Ŀǰ�ڼ�����������д�롰��ƬӲ����ʶ������ ��Ƭ�����롱���������������롱�Ĳ���Ӧ�ö���Ϊ�������ͼ�����ǰ�Ĳ��淶��Ƭ��
	// �������˽������뿨��ȷ�ϣ�����

	// Ŀǰ����ţ�У����Ϊ�ƿ�ʱд�룬���ڿ�Ƭ����ʱ�������޸ġ�
	// ������Ҫ�޸ĵ���ϢΪ��
	// 1. ��ƬӲ����ʶĬ��ֵΪ��FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF������Ҫ�ڼ���ʱд����ȷֵ��ͨ������������FindCard����ã���
	// 2. ��Ƭ������Ĭ��ֵΪ��100001������Ҫͨ������ż�����ò�д�롣����ž߱�һ���Ĺ����ԣ����������㿨Ƭ�����롣
	// 3. ������������Ĭ��ֵΪ��00000001������Ҫͨ������ż�����ò�д�룬����ŵ�ǰ4λ���ִ������������롣
	// ע�⣺����ǰ���ƿ������н����ֿ�Ƭ�ķ�����������������ڼ����������Ҫ�Է��������������У����
	// ��һ�������Ϊ8000��ͷ�Ŀ��ķ�����������Ӧ��У��Ϊ8888��
	void RWDevice::ActivateCard(const std::string& card_sn, 
								const std::string& key_DCCK, 
								const std::string& key_F0015_DACK, 
								const std::string& key_F0015_DEAK,
								const std::string& key_F0015_DAMK1) const
	{
		// ע�⣬����ʱ����һ����ֵ��Χ���忨Ƭ���͵��������зǳ���ľ����Եģ�
		// ���磬��Ϻ�����ֻ������2500�ţ����ǹ�����������������Ϊ�˼���ǰ�ڵ�һЩ�������淶�Ŀ�Ƭ��
		
		// �ڽ�.��Ϻ��8888 0003 0001 - 8888 0003 2500, �����ͱ��룺100001
		static const CardTypeCfg CARD_TYPE_LONGXIA = { "100001", 30001, 32500 }; // ��Ϻ��

		// ����.��ݮ��8888 0001 0001-8888 0001 2500, �����ͱ��룺100002
		static const CardTypeCfg CARD_TYPE_LANMEI = { "100002", 10001, 12500 }; // ��ݮ��

		// �ƽ�.���뿨8888 0002 0001-8888 0002 1000����һ���Σ�, �����ͱ��룺100003
		static const CardTypeCfg CARD_TYPE_MAISUI_BATCH1 = { "100003", 20001, 21000 }; // ���뿨

		// �ƽ�.���뿨8888 0002 1001-8888 0002 3000���ڶ����Σ�, �����ͱ��룺100003
		static const CardTypeCfg CARD_TYPE_MAISUI = { "100003", 21001, 23000 }; // ���뿨

		// ����.���Կ�8888 0000 0001-8888 0000 2500, �����ͱ��룺100004
		static const CardTypeCfg CARD_TYPE_DOUJIA = { "100004", 1, 2500 }; // ���Կ�

		const char kKeyID = NULL; // No need to do authentication.
		const std::string kKey = "";

		static const uint32_t CARD_SN_DATA_OFFSET = CARD_CHIP_NUM_LEN + CARD_TYPE_CODE_LEN + CARD_ISSUER_CODE_LEN + CARD_VERIFY_CODE_LEN;

		/*
		��������Ŀǰ��������԰���Ŀ������õ���԰��ʹ�ã������ʱȡ�������ơ�
		����Ҫע�������ԻὫ�ڱ�԰����������Ŀ������к�����ΪĬ��ֵ��FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF����

		// ��ȡ��Ƭ����ţ����жϸÿ��Ƿ���δ����״̬
		std::string card_sn_str = ReadCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, CARD_SN_DATA_OFFSET, CARD_SN_LEN, kKeyID, kKey);
		if (card_sn_str.compare(DEFAULT_CARD_DEVICE_ID) != 0)
			throw std::exception("Card already activated!");
		*/

		// ��ȡ�����
		static const uint32_t CHIP_NUM_DATA_OFFSET = 0;
		std::string chip_num_str = ReadCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, CHIP_NUM_DATA_OFFSET, CARD_CHIP_NUM_LEN, kKeyID, kKey);

		/*ע�⣺Ŀǰ��ȷ��Ϊĳһ�г����ƵĿ��Ƿ��ܹ��õ������г�ȥʹ�ã������ʱ�����˼�顣
		// ���鿨Ƭ�Ƿ�����ڵ�ǰ�г�ʹ��.
		const uint32_t kIssuerCodeLen = CARD_ISSUER_CODE_HAERBIN_HADA.size(); // �����ǰ4λ���ּ�Ϊ������������.
		std::string issuer_code_of_chip_num = chip_num_str.substr(0, kIssuerCodeLen);
		if (!(issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA) == 0 ||
			issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA_BATCH1) == 0))
			throw std::exception("This card belongs to other market, it cannot be used in this market!");
		*/

		// ��������
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, K_F0015_DACK_DV, K_F0015_DAMK1_CMD_HEAD, key_F0015_DAMK1.c_str(), false);
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, K_F0015_DACK_DV, K_F0015_DEAK_CMD_HEAD, key_F0015_DEAK.c_str(), false);
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, K_F0015_DACK_DV, K_F0015_DACK_CMD_HEAD, key_F0015_DACK.c_str(), true);
		SetPassword(MF_FILE_DIR, ACTIVATE_KEY_ID, K_DCCK_DV, K_DCCK_CMD_HEAD, key_DCCK.c_str(), true);

		// д��ƬӲ����ʶ����Ƭ����, �����к�SN��
		// ע������offset���㲻��Ҫ���С�/2��ת������Ϊ���API��������Ķ���HEX�ַ�����ʽ���ڡ�WriteCardFile���ڲ����Զ����С�/2��ת����
		WriteCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, CARD_SN_DATA_OFFSET, ACTIVATE_KEY_ID, K_F0005_DCMK1, card_sn);
		
		// ����������
		uint8_t issuer_code_len = CARD_ISSUER_CODE_HAERBIN_HADA.size();
		// ע������offset���㲻��Ҫ���С�/2��ת������Ϊ���API��������Ķ���HEX�ַ�����ʽ���ڡ�WriteCardFile���ڲ����Զ����С�/2��ת����
		static const uint32_t ISSUER_CODE_DATA_OFFSET = CARD_CHIP_NUM_LEN + CARD_TYPE_CODE_LEN;
		std::string issuer_code_by_read = ReadCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, ISSUER_CODE_DATA_OFFSET, CARD_ISSUER_CODE_LEN, kKeyID, kKey);
		issuer_code_by_read = issuer_code_by_read.substr(0, issuer_code_len);
		bool is_issuer_code_by_read_valid = issuer_code_by_read.compare(CARD_ISSUER_CODE_HAERBIN_HADA) == 0; // �����ķ�����������ǰ4Ϊ"8888".

		std::string issuer_code_of_chip_num = chip_num_str.substr(0, issuer_code_len);
		bool is_issuer_code_of_chip_num_valid = issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA) == 0 ||
			issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA_BATCH1) == 0; // �����ǰ4λΪ"8000"����"8888"��
		
		if (is_issuer_code_of_chip_num_valid && !is_issuer_code_by_read_valid) //������Ҫ��д������������
		{
			std::string issuer_code = CARD_ISSUER_CODE_HAERBIN_HADA + std::string(CARD_ISSUER_CODE_LEN - CARD_ISSUER_CODE_HAERBIN_HADA.size(), PADDING_CHAR);
			WriteCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, ISSUER_CODE_DATA_OFFSET, ACTIVATE_KEY_ID, K_F0005_DCMK1, issuer_code);
		}

		// д��Ƭ������
		// ��鿨�����ǰ4λ������ȷ���Ƿ���Ҫ����д�뿨���ͱ��룬�����ݿ�����к�5λ����ȷ����Ƭ����.
		std::string card_type_str(CARD_TYPE_CODE_LEN, PADDING_CHAR);
		static const uint8_t MARK_NUM_LEN = 5;
		uint32_t mark_num = std::atoi(chip_num_str.substr(CARD_CHIP_NUM_LEN - MARK_NUM_LEN, MARK_NUM_LEN).c_str());

		bool need_write_code_type = false;
		if (issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA_BATCH1) == 0) // ��Կ����ǰ4λΪ��8000���ĵ�һ���ο�
		{
			if (mark_num >= CARD_TYPE_MAISUI_BATCH1.mark_num_min && mark_num <= CARD_TYPE_MAISUI_BATCH1.mark_num_max)
			{
				card_type_str.replace(0, CARD_TYPE_MAISUI_BATCH1.code.size(), CARD_TYPE_MAISUI_BATCH1.code);
				need_write_code_type = true;
			}
		}
		else if (issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA) == 0) // ����Ժ����εĿ����ǰ4λΪ��8888���Ŀ�
		{
			if (mark_num >= CARD_TYPE_LONGXIA.mark_num_min && mark_num <= CARD_TYPE_LONGXIA.mark_num_max)
				card_type_str.replace(0, CARD_TYPE_LONGXIA.code.size(), CARD_TYPE_LONGXIA.code);
			else if (mark_num >= CARD_TYPE_LANMEI.mark_num_min && mark_num <= CARD_TYPE_LANMEI.mark_num_max)
				card_type_str.replace(0, CARD_TYPE_LANMEI.code.size(), CARD_TYPE_LANMEI.code);
			else if (mark_num >= CARD_TYPE_MAISUI.mark_num_min && mark_num <= CARD_TYPE_MAISUI.mark_num_max)
				card_type_str.replace(0, CARD_TYPE_MAISUI.code.size(), CARD_TYPE_MAISUI.code);
			else if (mark_num >= CARD_TYPE_DOUJIA.mark_num_min && mark_num <= CARD_TYPE_DOUJIA.mark_num_max)
				card_type_str.replace(0, CARD_TYPE_DOUJIA.code.size(), CARD_TYPE_DOUJIA.code);
			need_write_code_type = true;
		}
		// ע������offset���㲻��Ҫ���С�/2��ת������Ϊ���API��������Ķ���HEX�ַ�����ʽ���ڡ�WriteCardFile���ڲ����Զ����С�/2��ת����
		if (need_write_code_type) // ������Ҫ����д�뿨Ƭ���ͱ���.
		{
			static const uint32_t TYPE_CODE_DATA_OFFSET = CARD_CHIP_NUM_LEN;
			WriteCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, TYPE_CODE_DATA_OFFSET, ACTIVATE_KEY_ID, K_F0005_DCMK1, card_type_str);
		}

		//��ҵ����Ϣ�ļ��е����ݳ�������Ϊ0.
		WriteCardFile(DF01_DIR, SERVICE_INFO_FILE_ID, 0/*offset*/, ACTIVATE_KEY_ID, key_F0015_DAMK1, DecToHex(0)); // Default length is 0.
	}

	void RWDevice::ResetCard(const std::string& key_DCCK, const std::string& key_F0015_DACK) const
	{
		/*ע�⣺Ŀǰ��ȷ��Ϊĳһ�г����ƵĿ��Ƿ��ܹ��õ������г�ȥʹ�ã������ʱ�����˼�顣
		// ���鿨Ƭ�Ƿ�����ڵ�ǰ�г�ʹ��.
		// ��ȡ�����
		const char kKeyID = NULL; // No need to do authentication.
		const std::string kKey = "";
		static const uint32_t CHIP_NUM_DATA_OFFSET = 0;
		std::string chip_num_str = ReadCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, CHIP_NUM_DATA_OFFSET, CARD_CHIP_NUM_LEN, kKeyID, kKey);

		const uint32_t kIssuerCodeLen = CARD_ISSUER_CODE_HAERBIN_HADA.size(); // �����ǰ4λ���ּ�Ϊ������������.
		std::string issuer_code_of_chip_num = chip_num_str.substr(0, kIssuerCodeLen);
		if (!(issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA) == 0 ||
		issuer_code_of_chip_num.compare(CARD_ISSUER_CODE_HAERBIN_HADA_BATCH1) == 0))
		throw std::exception("This card belongs to other market, it cannot be used in this market!");
		*/


		// ��������
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, key_F0015_DACK.c_str(), K_F0015_DAMK1_CMD_HEAD, K_F0015_DAMK1_DV, false);
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, key_F0015_DACK, K_F0015_DEAK_CMD_HEAD, K_F0015_DEAK_DV, false);
		SetPassword(DF01_DIR, ACTIVATE_KEY_ID, key_F0015_DACK, K_F0015_DACK_CMD_HEAD, K_F0015_DACK_DV, true);
		SetPassword(MF_FILE_DIR, ACTIVATE_KEY_ID, key_DCCK.c_str(), K_DCCK_CMD_HEAD, K_DCCK_DV, true);

		// ��ֵ��Ƭ����
		// ע������offset���㲻��Ҫ���С�/2��ת������Ϊ���API��������Ķ���HEX�ַ�����ʽ���ڡ�WriteCardFile���ڲ����Զ����С�/2��ת����
		static const uint32_t CARD_SN_DATA_OFFSET = CARD_CHIP_NUM_LEN + CARD_TYPE_CODE_LEN + CARD_ISSUER_CODE_LEN + CARD_VERIFY_CODE_LEN;
		WriteCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, CARD_SN_DATA_OFFSET, ACTIVATE_KEY_ID, K_F0005_DCMK1, DEFAULT_CARD_DEVICE_ID);
		
		//��ҵ����Ϣ�ļ��е����ݳ�������Ϊ0.
		WriteCardFile(DF01_DIR, SERVICE_INFO_FILE_ID, 0/*offset*/, ACTIVATE_KEY_ID, K_F0015_DAMK1_DV, DecToHex(0)); // Default length is 0.
	}

	BasicInfo RWDevice::ReadCardBasicInfo() const
	{
		const uint32_t kOffset = 0;
		const uint32_t kLength = CARD_CHIP_NUM_LEN + CARD_SN_LEN + CARD_TYPE_CODE_LEN + CARD_ISSUER_CODE_LEN + CARD_VERIFY_CODE_LEN;
		const char kKeyID = NULL; // No need to do authentication.
		const std::string kKey = "";
		std::string basic_info_str = ReadCardFile(MF_FILE_DIR, BASIC_INFO_FILE_ID, kOffset, kLength, kKeyID, kKey);

		BasicInfo basic_info;
		
		uint32_t offset = 0;
		basic_info.chip_num = basic_info_str.substr(offset, CARD_CHIP_NUM_LEN);
		offset += CARD_CHIP_NUM_LEN;

		std::string field;
		field = basic_info_str.substr(offset, CARD_TYPE_CODE_LEN);
		basic_info.type_code = field.substr(0, field.find_first_of(PADDING_CHAR));
		offset += CARD_TYPE_CODE_LEN;

		field = basic_info_str.substr(offset, CARD_ISSUER_CODE_LEN);
		basic_info.issuer_code = field.substr(0, field.find_first_of(PADDING_CHAR));
		offset += CARD_ISSUER_CODE_LEN;

		field = basic_info_str.substr(offset, CARD_VERIFY_CODE_LEN);
		basic_info.verify_code = field.substr(0, field.find_first_of(PADDING_CHAR));
		offset += CARD_VERIFY_CODE_LEN;

		basic_info.device_id = basic_info_str.substr(offset, CARD_SN_LEN);

		return basic_info;
	}
	
	std::string RWDevice::ReadCardServiceInfo(const std::string& key_F0015_DEAK) const
	{
		const uint32_t kOffset = 0;
		std::string data_len_hex = ReadCardFile(DF01_DIR, 
			SERVICE_INFO_FILE_ID, kOffset, SERVICE_INFO_DATA_HEADER_LEN * 2, SERVICE_INFO_KEY_ID, key_F0015_DEAK);
		uint32_t data_len = _byteswap_ulong(std::stoul(data_len_hex, 0, 16)); // ���תС��.
		if (data_len > CARD_DATA_LEN_MAX)
			throw std::exception("Read card service info data length failed��");
		if (0 == data_len)
			return "";
		else
			// ע������offset���㲻��Ҫ���С�/2��ת������Ϊ���API��������Ķ���HEX�ַ�����ʽ���ڡ�WriteCardFile���ڲ����Զ����С�/2��ת����
			return ReadCardFile(DF01_DIR, 
				SERVICE_INFO_FILE_ID, data_len_hex.size(), data_len * 2, SERVICE_INFO_KEY_ID, key_F0015_DEAK); // ���ݳ��ȶ�����.
	}

	void RWDevice::WriteCardServiceInfo(const std::string& key_F0015_DAMK1, const std::string& data) const
	{
		uint32_t data_len = data.size() / 2; // Why "/ 2"? refer to the comment at the front of this file.
		if (data_len > SERVICE_INFO_DATA_LEN_MAX)
			throw std::exception("Data length too long, write card service info data failed�� Max valid length: 250 bytes!)");
		std::string data_len_str = DecToHex(_byteswap_ulong(data_len)); // Convert to big endian first and then to HEX string.
		WriteCardFile(DF01_DIR, SERVICE_INFO_FILE_ID, 0, SERVICE_INFO_KEY_ID, key_F0015_DAMK1, data_len_str + data);
	}

	//------Private Implementations----------------
	void RWDevice::Init()
	{
		const std::wstring kDLLFile = L"./dyn.dll";
		HINSTANCE dll_handle = LoadLibrary(kDLLFile.c_str());
		if (NULL != dll_handle)
		{
			m_fn_open = (OpenFn)GetProcAddress(dll_handle, "open");
			m_fn_beep = (BeepFn)GetProcAddress(dll_handle, "beep");
			m_fn_close = (CloseFn)GetProcAddress(dll_handle, "close");
			m_fn_find_card = (FindCardFn)GetProcAddress(dll_handle, "findCard");
			m_fn_write_bin_data = (WriteBinaryDataFn)GetProcAddress(dll_handle, "write_bin_file");
			m_fn_get_value = (GetValueFn)GetProcAddress(dll_handle, "get_value_char"); // Don't use "get_value" version!!!
			m_fn_ch_dir = (ChangeDirFn)GetProcAddress(dll_handle, "chang_dir");
			m_fn_read_bin_data = (ReadBinaryDataFn)GetProcAddress(dll_handle, "read_bin_file");
			m_fn_ex_auth = (ExternalAuthFn)GetProcAddress(dll_handle, "External_Authentication");
			m_fn_install_key = (InstallKeyFn)GetProcAddress(dll_handle, "InstallKey");
		}
		else
			throw std::exception(("Fail to load the DILI card device DLL from path : " + wstr_2_str(kDLLFile)).c_str());
	}

	std::string RWDevice::GetValue() const
	{
		char val[DEFAULT_DATA_BLOCK_LEN] = { 0 };
		m_fn_get_value(&val[0]);
		return val;
	}

	std::string RWDevice::ReadCardFile(const std::string&	dir, 
									   char					file_id, 
									   uint32_t				offset, 
									   uint32_t				length, 
									   char					key_id, 
									   const std::string&	key) const
	{
		std::stringstream err_msg;
		if (!m_fn_ch_dir(dir.c_str())) // First change to specified directory.
		{
			err_msg << "Change directory failed, DIR: " << dir << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}
		if (NULL != key_id) // If authentication is needed.
		{
			if (!m_fn_ex_auth(key_id, key.c_str()))
			{
				err_msg << "Authenticate file failed, FILE: " << file_id << ", ErrorCode: " << GetValue();
				throw std::exception(err_msg.str().c_str());
			}
		}
		// Read binary data from card.  
		if (!m_fn_read_bin_data(file_id, offset / 2, length / 2)) // Why "/ 2"? refer to the comment at the front of this file.
		{
			err_msg << "Read file failed, FILE: " << file_id << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}
		return GetValue();
	}

	void RWDevice::WriteCardFile(const std::string& dir, 
								 char				file_id, 
								 uint32_t			offset, 
								 char				key_id, 
								 const std::string& key, 
								 const std::string& data) const
	{
		std::stringstream err_msg;
		if (!m_fn_ch_dir(dir.c_str())) // First change to specified directory.
		{
			err_msg << "Change directory failed, DIR: " << dir << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}

		uint32_t length = data.size();
		uint32_t written_data_size = 0;
		while (written_data_size < length)
		{
			if (NULL != key_id) // If authentication is needed.
			{
				if (!m_fn_ex_auth(key_id, key.c_str()))
				{
					err_msg << "Authenticate file failed, FILE: " << file_id << ", ErrorCode: " << GetValue();
					throw std::exception(err_msg.str().c_str());
				}
			}
			std::string data_block = data.substr(written_data_size, WRITE_DATA_BLOCK_LEN * 2); // Why "* 2"? refer to the comment at the front of this file.
			uint32_t block_size = data_block.size();
			if (!m_fn_write_bin_data(file_id, (offset + written_data_size) / 2, block_size / 2, key.c_str(), data_block.c_str())) // Why "/ 2"? refer to the comment at the front of this file.
			{
				err_msg << "Write data to file failed, FILE: " << file_id << ", ErrorCode: " << GetValue();
				throw std::exception(err_msg.str().c_str());
			}
			written_data_size += block_size;
		}
	}

	void RWDevice::SetPassword(const std::string&	dir, 
							   char					key_id, 
							   const std::string&	main_key, 
							   const std::string&	cmd_head, 
							   const std::string&	install_key, 
							   bool					is_main_key) const
	{
		std::stringstream err_msg;
		if (!m_fn_ch_dir(dir.c_str())) // First change to specified directory.
		{
			err_msg << "Change directory failed, DIR: " << dir << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}
		if (!m_fn_ex_auth(key_id, main_key.c_str()))
		{
			err_msg << "Authenticate key failed, original key: " << main_key << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}
		if (!m_fn_install_key(main_key.c_str(), cmd_head.c_str(), install_key.c_str(), is_main_key, true))
		{
			err_msg << "Update key failed, key to be updated: " << install_key << ", ErrorCode: " << GetValue();
			throw std::exception(err_msg.str().c_str());
		}
	}
} // DILICard