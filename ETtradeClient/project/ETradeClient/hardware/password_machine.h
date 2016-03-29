#ifndef ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED
#define ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED

/*Password machine is used to calculate the password for verifying the DILI card.
*/

/*
密码机信息：
生产厂商：广州江南科友科技股份有限公司
型号：（未知）
*/

#include <string>
#include <cstdint>

class PWDMachine
{
public:
	PWDMachine();
	~PWDMachine();
	bool Connect(const std::string& ip, uint32_t port) const;
	void Disconnect() const;
	std::string GetCardPassword(const std::string&	key_idx, // 密钥索引
								const std::string&	card_sn, // 卡序列号（卡片内码），用作离散数据
								uint32_t			timeout) const; // 单位：毫秒
private:
	mutable int32_t m_socket;
}; // PWDMachine

#endif // ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED
