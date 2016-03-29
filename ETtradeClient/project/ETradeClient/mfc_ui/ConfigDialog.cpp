// SetAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfigDialog.h"
#include "afxdialogex.h"

#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/string_converter.h"


IMPLEMENT_DYNAMIC(CConfigDialog, CDialog)

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &CConfigDialog::OnCbnSelchangeComboProtocol)
END_MESSAGE_MAP()

CConfigDialog::CConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{}

static const int HTTP = 0, HTTPS = 1;

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	const int kMinPortValue = 0, kMaxPortValue = 65535;
	const int kMinMsgCountQueryInterval = 60, kMaxMsgCountQueryInterval = 3600;
	const int kMinTimeoutMs = 100, kMaxTimeoutMs = 10000;
	const int kMinTimeoutSeconds = 1, kMaxTimeoutSeconds = 10;
	const int kMinPwdLen = 6, kMaxPwdLen = 32;

	DDX_Text(pDX, IDC_EDIT_MSG_COUNT_QUERY_INTERVAL, m_msg_count_query_interval);
	DDV_MinMaxInt(pDX, m_msg_count_query_interval, kMinMsgCountQueryInterval, kMaxMsgCountQueryInterval);

	DDX_CBIndex(pDX, IDC_COMBO_PROTOCOL, m_protocol_index);

	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_server_port);
	DDV_MinMaxInt(pDX, m_server_port, kMinPortValue, kMaxPortValue);

	DDX_Text(pDX, IDC_IP_ADDR_PWD_MACHINE, m_pwd_machine_ip);

	DDX_Text(pDX, IDC_EDIT_PWD_MACHINE_PORT, m_pwd_machine_port);
	DDV_MinMaxInt(pDX, m_pwd_machine_port, kMinPortValue, kMaxPortValue);

	DDX_Text(pDX, IDC_EDIT_PWD_MACHINE_TIMEOUT, m_pwd_machine_timeout);
	DDV_MinMaxInt(pDX, m_pwd_machine_timeout, kMinTimeoutMs, kMaxTimeoutMs);

	DDX_Text(pDX, IDC_EDIT_PIN_PAD_LEN, m_pin_pad_pwd_len);
	DDV_MinMaxInt(pDX, m_pin_pad_pwd_len, kMinPwdLen, kMaxPwdLen);

	DDX_Text(pDX, IDC_EDIT_PIN_PAD_TIMEOUT, m_pin_pad_timeout);
	DDV_MinMaxInt(pDX, m_pin_pad_timeout, kMinTimeoutSeconds, kMaxTimeoutSeconds);
}

void CConfigDialog::OnCbnSelchangeComboProtocol()
{
	if (!UpdateData(TRUE))// 检查输入数据合法性，并存入关联变量
		return;
	const int kDefaultHttpsPort = 443, kDefaultHttpPort = 80;
	m_server_port = (HTTPS == m_protocol_index) ? kDefaultHttpsPort : kDefaultHttpPort;
	UpdateData(FALSE);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 取出配置，并更新到界面
	const ApplicationConfig& app_config = ApplicationConfig::Instance();
	m_msg_count_query_interval = app_config.MsgCountQueryInterval();
	m_pwd_machine_ip		= str_2_wstr(app_config.PWDMachineIP()).c_str();
	m_pwd_machine_port		= app_config.PWDMachinePort();
	m_pwd_machine_timeout	= app_config.PWDMachineTimeout();
	m_pin_pad_pwd_len		= app_config.PINPadPWDLen();
	m_pin_pad_timeout		= app_config.PINPadTimeout();

	const URLConfig& url_config = URLConfig::Instance();
	m_protocol_index = url_config.IsHttps() ? HTTPS : HTTP;
	m_server_port = url_config.Port();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CConfigDialog::OnOK()
{
	if (!UpdateData(TRUE))// 检查输入数据合法性，并存入关联变量
		return;

	// 保存配置的值
	ApplicationConfig::Instance().ModifyCfg(
		m_msg_count_query_interval, 
		wstr_2_str(m_pwd_machine_ip.GetBuffer(m_pwd_machine_ip.GetLength())),
		m_pwd_machine_port,
		m_pwd_machine_timeout,
		m_pin_pad_pwd_len,
		m_pin_pad_timeout);
	URLConfig::Instance().ModifyCfg((HTTPS == m_protocol_index), m_server_port);

	return CDialog::OnOK();
}
