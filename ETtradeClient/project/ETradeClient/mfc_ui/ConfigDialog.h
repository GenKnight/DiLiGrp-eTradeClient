#pragma once

#include <cstdint>

#include "resource.h"

class CConfigDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfigDialog)

public:
	CConfigDialog(CWnd* pParent = NULL);   // ��׼���캯��

	virtual BOOL OnInitDialog();
	virtual void OnOK();

// �Ի�������
	enum { IDD = IDD_MODIFY_CONFIG };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnCbnSelchangeComboProtocol();

	DECLARE_MESSAGE_MAP()

private:
	int				m_msg_count_query_interval;

	int				m_protocol_index;
	int				m_server_port;

	CString			m_pwd_machine_ip;
	int				m_pwd_machine_port;
	int				m_pwd_machine_timeout;

	uint8_t			m_pin_pad_pwd_len;
	uint8_t			m_pin_pad_timeout;
};
