#include "stdafx.h"

#include "etradeclient/hardware/password_machine.h"

#include <sstream>

#include "etradeclient/hardware/ex_reference/pwd_machine/mangerCmd.h"
#include "etradeclient/hardware/ex_reference/pwd_machine/commonCmd.h"

PWDMachine::PWDMachine() : m_socket(-1) // "-1" indicates a invalid socket.
{}

PWDMachine::~PWDMachine()
{
	Disconnect();
}

bool PWDMachine::Connect(const std::string& ip, uint32_t port) const
{
	m_socket = union_syj1001_connect(const_cast<char*>(ip.c_str()), port, 0); // 该函数的连接超时为20秒左右，因此不做重试.
	return m_socket > 0;
}

void PWDMachine::Disconnect() const
{
	if (m_socket > 0)
	{
		if (union_syj1001_disConnect(m_socket) < 0)
			throw std::exception("Disconnect password machine failed.");
	}
	m_socket = -1; // Reset.
}

std::string PWDMachine::GetCardPassword(const std::string&	key_idx,
										const std::string&	card_sn,
										uint32_t			timeout) const
{
	static const uint8_t MK_KEY_VAL = NULL;
	static const uint8_t DISPERSE_COUNT = 2; // 离散次数
	static const uint8_t NEED_READ_CFG = 0; // 0:不从配置文件读取hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg.
	static const uint8_t MSG_HEADER_LEN = 8; // hsmLenOfMsgHeader.
	static const uint8_t NEED_HSM_MSG_LEN = 1; // 是否需要报文长度 0：不需要 1：需要.
	static const uint8_t DISPERSE_VAR_LEN = 16; // 离散变量的最大单位长度为16个字符.

	static const uint8_t KEY_VAL_LEN = 32;
	typedef char NewKeyVal[KEY_VAL_LEN]; // MK加密下的过程密钥/子密钥密文, 长度为32个字符的字符数组。

	static const uint8_t CHECK_VAL_LEN = 16;
	typedef char ChkVal[CHECK_VAL_LEN]; // 过程密钥/子密钥的校验值, 长度为16个字符的字符数组。

	if (card_sn.empty())
		throw std::exception("Empty card serial number!");

	NewKeyVal new_key = { 0 };
	ChkVal chk_val = { 0 };
	int32_t key_len = union_syj1001_disperseKey(NULL, 0, timeout, NEED_READ_CFG, MSG_HEADER_LEN, NEED_HSM_MSG_LEN, m_socket, 
		const_cast<char*>(key_idx.c_str()), MK_KEY_VAL, new_key, chk_val, DISPERSE_COUNT,
		card_sn.substr(0, DISPERSE_VAR_LEN).c_str(), card_sn.substr(DISPERSE_VAR_LEN, DISPERSE_VAR_LEN).c_str()); // 离散变量如果大于16字符，要把离散量拆成数组.

	if (key_len < 0)
	{
		std::stringstream err_msg;
		err_msg << "Get password failed, KEY index: " << key_idx << ", Card SN: " << card_sn << ".";
		throw std::exception(err_msg.str().c_str());
	}
	return std::string(new_key, key_len);
}