#ifndef ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED
#define ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED

/*Password machine is used to calculate the password for verifying the DILI card.
*/

/*
�������Ϣ��
�������̣����ݽ��Ͽ��ѿƼ��ɷ����޹�˾
�ͺţ���δ֪��
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
	std::string GetCardPassword(const std::string&	key_idx, // ��Կ����
								const std::string&	card_sn, // �����кţ���Ƭ���룩��������ɢ����
								uint32_t			timeout) const; // ��λ������
private:
	mutable int32_t m_socket;
}; // PWDMachine

#endif // ETRADECLIENT_HARDWARE_PASSWORD_MACHINE_H_INCLUDED
