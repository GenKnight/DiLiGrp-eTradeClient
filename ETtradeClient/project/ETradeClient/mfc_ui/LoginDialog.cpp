// LoginDialog.cpp : implementation file
//

#include "stdafx.h"

#include "ETradeClient.h"
#include "LoginDialog.h"
#include "afxdialogex.h"

#include <string>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>


#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.

#include "etradeclient/utility/logon_mgr.h"

#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/application_config.h"


#include "etradeclient/utility/session.h"
#include "etradeclient/browser/embedded_browser.h"
#include "etradeclient/utility/string_converter.h"
#include "etradeclient/utility/win_http.h"
#include "etradeclient/utility/logging.h"

//namespace fs = boost::filesystem;

namespace
{
	static const std::string LOGIN_TYPE_USER_NAME = "user_name";
	static const std::string LOGIN_TYPE_ACCOUNT_NO = "account_no";
}

// CLoginDialog dialog

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CLoginDialog::OnLoginBtnClicked)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_STN_CLICKED(IDC_STATIC_LOGIN_TYPE, &CLoginDialog::OnStnClickedStaticLoginType)
END_MESSAGE_MAP()

CLoginDialog::CLoginDialog(const std::wstring& title, CWnd* pParent)
	: CDialogEx(CLoginDialog::IDD, pParent), m_title(title), m_login_type(USER_NAME), m_account_edit(L"用户名"), m_pwd_edit(L"密码", true)
{}

CLoginDialog::~CLoginDialog()
{}

bool CLoginDialog::Launch()
{
	if (IDCANCEL == DoModal())
		return false;
	return true;
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCOUNT_EDIT, m_account_edit);
	DDX_Control(pDX, IDC_PWD_EDIT, m_pwd_edit);
	DDX_Control(pDX, IDC_LOGIN_ERR_MSG, m_login_err_msg_text);
	DDX_Control(pDX, IDC_STATIC_LOGIN_TYPE, m_login_type_text);
}

// CLoginDialog message handlers
BOOL CLoginDialog::OnInitDialog()
{
	ModifyStyleEx(0, WS_EX_APPWINDOW); // This enables minimizing the dialog into taskbar instead of bottom left corner.
	CDialogEx::OnInitDialog();

	HICON dlg_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Specifies a 32 pixel by 32 pixel icon if TRUE; specifies a 16 pixel by 16 pixel icon if FALSE.
	SetIcon(dlg_icon, TRUE); // Set big icon
	SetIcon(dlg_icon, FALSE); // Set small icon

	//Load the backgroud image just only once.
	if (!LoadBackgroundImage())
	{
		LOG_ERROR(L"加载背景图片文件出错，请检查！");
		return FALSE;
	}
	m_login_btn.Create(_T("登录"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(), this, IDOK);

	AdjustDlgSize();
	SetSubCtrlStyle();
	SetSubCtrlLayout();

	SetWindowText(m_title.c_str());
	if (!Session::Instance().UserName().empty())
	{
		m_account_edit.SetText(Session::Instance().UserName().c_str());
		m_account_edit.SetReadOnly();
		m_pwd_edit.SetFocus();
	}
	else
		m_login_btn.SetFocus();

	//Invalidate(TRUE);// 重绘界面，解决界面控件显示不全的问题
	SwitchToThisWindow(GetSafeHwnd(), TRUE);
	return false;  // return TRUE unless you set the focus to a control
}

void CLoginDialog::OnPaint()
{
	// Do not call CDialogEx::OnPaint() for painting messages
	CPaintDC dc(this); // Device context for painting
	CDC mem_dc; // Memory device context
	mem_dc.CreateCompatibleDC(&dc); // Makes compatible with CPaintDC
	mem_dc.SelectObject(m_bg_img); // Selects img into CDC
	CRect client_rect;
	GetClientRect(&client_rect);
	SetStretchBltMode(dc, STRETCH_HALFTONE);
	dc.StretchBlt(0, 0, client_rect.Width(), client_rect.Height(), &mem_dc, 
		0, 0, m_bg_img.GetWidth(), m_bg_img.GetHeight(), SRCCOPY);
}

void CLoginDialog::OnLoginBtnClicked()
{
	CRect rect;
	m_login_err_msg_text.GetWindowRect(rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect); // Refresh error message text rect when next time get "WM_PAINT" event.

	CString account = m_account_edit.GetText();
	CString pwd = m_pwd_edit.GetText();
	if (account.IsEmpty() || pwd.IsEmpty())
	{
		m_login_err_msg_text.SetWindowTextW(L"用户名或密码不能为空！");
		return;
	}

	LogonMgr& logon_mgr = LogonMgr::Instance();
	if (!logon_mgr.DoLogin(std::wstring(account), std::wstring(pwd), 
		USER_NAME == m_login_type ? LOGIN_TYPE_USER_NAME : LOGIN_TYPE_ACCOUNT_NO)) // If log in failed.
	{
		m_login_err_msg_text.SetWindowTextW(logon_mgr.ErrorMsg().c_str());
		LOG_ERROR(L"登录系统失败: " + logon_mgr.ErrorMsg() + L"!");
		return;
	}
	LOG_TRACE(L"登录系统成功。");
	if (Session::Instance().UserName().empty())
		Session::Instance().SetUserName(std::wstring(account));

	const WinHttp::Cookies& cookies = Session::Instance().Cookies();
	for (const WinHttp::Cookie& cookie : cookies) // Set cookie to CEF.
	{
		CefCookie cef_cookie;
		CefString(&cef_cookie.name) = cookie.name;
		CefString(&cef_cookie.value) = cookie.value;
		CefString(&cef_cookie.domain) = cookie.domain;
		CefString(&cef_cookie.path) = cookie.path;
		cef_cookie.secure = cookie.secure;
		cef_cookie.httponly = cookie.httponly;

		if (!EmbeddedBrower::SetCookie(URLConfig::Instance().FullHost(), cef_cookie)) // If set cookie failed.
		{
			LOG_ERROR(L"设置Cookie失败，网络通信异常，请关闭程序重新登录！");
			m_login_err_msg_text.SetWindowTextW(L"网络通信异常，请关闭程序重新登录！");
			return;
		}	
	}
	Session::Instance().OnValid(); // If login succeeded, recover the session state to be "not expired".

	CDialogEx::OnOK();
}

HBRUSH CLoginDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// 设置错误提示信息文字颜色。注：先调用 "CDialogEx::OnCtlColor" 否则不会生效。
	if (IDC_LOGIN_ERR_MSG == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(200, 0, 0));
		return HBRUSH(GetStockObject(HOLLOW_BRUSH));
	}
	if (IDC_ACCOUNT_EDIT == pWnd->GetDlgCtrlID())
	{
		CEdit* edit_ctrl = dynamic_cast<CEdit*>(pWnd);
		if(edit_ctrl->GetStyle() & ES_READONLY)
			pDC->SetTextColor(RGB(100, 100, 100));
	}
	return hbr;
}

void CLoginDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd* parent = this->GetParent();
	if (NULL != parent)
	{
		if (SC_MINIMIZE == nID)
			parent->ShowWindow(SW_MINIMIZE);
		if (SC_RESTORE == nID)
			parent->ShowWindow(SW_RESTORE);
	}
	CDialogEx::OnSysCommand(nID, lParam);
}

void CLoginDialog::SetSubCtrlLayout()
{
	const int kSubCtrlWidth = 300, kSubCtrlHeight = 45, kErrMsgHeight = 20, kLoginTypeTextHeight = 20;
	const int kIntervalAccountPwd = 20, kIntervalPwdErrMsg = 10, kIntervalErrMsgOkBtn = 15, kIntervalOkBtnTypeText = 20;
	const int kSubCtrlPosX = 840;
	int kSubCtrlPosY = 185;

	m_account_edit.MoveWindow(kSubCtrlPosX, kSubCtrlPosY, kSubCtrlWidth, kSubCtrlHeight);
	kSubCtrlPosY += (kSubCtrlHeight + kIntervalAccountPwd);
	m_pwd_edit.MoveWindow(kSubCtrlPosX, kSubCtrlPosY, kSubCtrlWidth, kSubCtrlHeight);
	kSubCtrlPosY += kSubCtrlHeight + kIntervalPwdErrMsg;
	m_login_err_msg_text.MoveWindow(kSubCtrlPosX, kSubCtrlPosY, kSubCtrlWidth, kErrMsgHeight);
	kSubCtrlPosY += kErrMsgHeight + kIntervalErrMsgOkBtn;
	m_login_btn.MoveWindow(kSubCtrlPosX, kSubCtrlPosY, kSubCtrlWidth, kSubCtrlHeight);
	kSubCtrlPosY += kSubCtrlHeight + kIntervalOkBtnTypeText;
	m_login_type_text.MoveWindow(kSubCtrlPosX, +kSubCtrlPosY, kSubCtrlWidth, kLoginTypeTextHeight);
}

void CLoginDialog::SetSubCtrlStyle()
{
	const std::wstring kFontName = L"微软雅黑";
	const int kInputFontHeight = 20;
	const int kBtnFontHeight = 25;
	m_input_font.CreateFont(kInputFontHeight, // nHeight
		0,           // nWidth
		0,           // nEscapement
		0,           // nOrientation
		FW_THIN,     // nWeight
		FALSE,       // bItalic
		FALSE,       // bUnderline
		0,           // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		kFontName.c_str());        // lpszFac
	m_account_edit.SetFont(&m_input_font, FALSE);
	m_pwd_edit.SetFont(&m_input_font,FALSE);

	m_btn_font.CreateFont(kBtnFontHeight, // nHeight
		 0,           // nWidth
		 0,           // nEscapement
		 0,           // nOrientation
		 FW_NORMAL,   // nWeight
		 FALSE,       // bItalic
		 FALSE,       // bUnderline
		 0,           // cStrikeOut
		 ANSI_CHARSET,              // nCharSet
		 OUT_DEFAULT_PRECIS,        // nOutPrecision
		 CLIP_DEFAULT_PRECIS,       // nClipPrecision
		 DEFAULT_QUALITY,           // nQuality
		 DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		 kFontName.c_str());        // lpszFac
	m_login_btn.SetFont(&m_btn_font,FALSE);
}

void CLoginDialog::AdjustDlgSize()
{
	const int kDlgCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
	const int kDlgCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;

	CRect dlg_rect;
	GetWindowRect(&dlg_rect);
	CRect client_rect;
	GetClientRect(&client_rect);

	const int kDlgWidth = m_bg_img.GetWidth() + ((dlg_rect.right - dlg_rect.left) - (client_rect.right - client_rect.left));
	const int kDlgHeight = m_bg_img.GetHeight() + ((dlg_rect.bottom - dlg_rect.top) - (client_rect.bottom - client_rect.top));

	dlg_rect.left	= kDlgCenterX - kDlgWidth / 2;
	dlg_rect.right	= kDlgCenterX + kDlgWidth / 2;
	dlg_rect.top	= kDlgCenterY - kDlgHeight / 2;
	dlg_rect.bottom = kDlgCenterY + kDlgHeight / 2;

	MoveWindow(dlg_rect);
}

BOOL CLoginDialog::LoadBackgroundImage()
{
	namespace fs = boost::filesystem;
	const std::wstring login_bkg_img(L"login-bg.png");
	fs::path curr_exec_path = fs::current_path();
	curr_exec_path += L"\\Resource\\Login\\";
	std::wstring file_path = curr_exec_path.wstring() + login_bkg_img;
	if (!fs::exists(file_path))
		return FALSE;

	m_bg_img.Load(file_path.c_str());
	return TRUE;
}

// LoginBtn
void CLoginDialog::LoginBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* dc	= CDC::FromHandle(lpDrawItemStruct->hDC);
	UINT state	= lpDrawItemStruct->itemState;;
	CRect btn_rect, text_rc, offset_text_rect;
	btn_rect.CopyRect(&lpDrawItemStruct->rcItem);

	const int kOffset = 1;
	text_rc = btn_rect;
	text_rc.OffsetRect(-kOffset, -kOffset);
	offset_text_rect = text_rc;
	offset_text_rect.OffsetRect(kOffset, kOffset);

	// Retrieve the button's caption
	CString caption_str;
	GetWindowText(caption_str);

	//TODO: read from config file.
	const COLORREF kBtnRectEnable(RGB(35, 172, 56)), kBtnRectSelected(RGB(51, 185, 76)); //#23ac38, #33B94C
	const COLORREF kBtnTextEnable(RGB(204, 223, 237)), kBtnTextSelected(RGB(213, 228, 240));

	if (state & ODS_SELECTED)
	{
		DrawFilledRect(dc, btn_rect, kBtnRectSelected);
		DrawButtonText(dc, offset_text_rect, caption_str, kBtnTextSelected);
	}
	else
	{
		DrawFilledRect(dc, btn_rect, kBtnRectEnable);
		DrawButtonText(dc, text_rc, caption_str, kBtnTextEnable);
	}
}

void CLoginDialog::LoginBtn::DrawFilledRect(CDC* dc, CRect rc, COLORREF color)
{
	CBrush solid_br;

	solid_br.CreateSolidBrush(color);
	dc->FillRect(rc, &solid_br);
}

void CLoginDialog::LoginBtn::DrawButtonText(CDC* dc, CRect rc, CString caption, COLORREF textcolor)
{
	const CSize text_size = dc->GetOutputTextExtent(caption);
	const int kVerticalModifiedValue = 1;
	const int kVerticalStartPos = (rc.Height() - text_size.cy) / 2 - kVerticalModifiedValue;
	const uint32_t kDrawStyles = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	dc->SetTextColor(textcolor);
	dc->SetBkMode(TRANSPARENT);

	const int kHorizontalModifiedValue = 3;
	CRect text_rect = rc;
	text_rect.DeflateRect(kHorizontalModifiedValue, 0, kHorizontalModifiedValue, 0);
	text_rect.top = kVerticalStartPos;
	text_rect.bottom = kVerticalStartPos + text_size.cy;

	dc->DrawText(caption, caption.GetLength(), text_rect, kDrawStyles);
}

void CLoginDialog::OnStnClickedStaticLoginType()
{
	CRect rect_type;
	m_login_type_text.GetWindowRect(rect_type);
	ScreenToClient(&rect_type);
	InvalidateRect(&rect_type); // Refresh error message text rect when next time get "WM_PAINT" event.

	CRect rect_acc;
	m_account_edit.GetWindowRect(rect_acc);
	ScreenToClient(&rect_acc);
	InvalidateRect(&rect_acc); // Refresh error message text rect when next time get "WM_PAINT" event.

	if (USER_NAME == m_login_type)
	{
		m_login_type = CARD_NO;
		m_login_type_text.SetWindowTextW(L"点击此处，以用户名登录。");
		m_account_edit.SetDefaultText(L"卡账号");
	}
	else if (CARD_NO == m_login_type)
	{
		m_login_type = USER_NAME;
		m_login_type_text.SetWindowTextW(L"点击此处，以卡账号登录。");
		m_account_edit.SetDefaultText(L"用户名");
	}
}