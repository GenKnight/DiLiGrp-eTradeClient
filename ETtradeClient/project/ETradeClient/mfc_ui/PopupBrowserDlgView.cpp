#include "stdafx.h"

#include "etradeclient/mfc_ui/PopupBrowserDlgView.h"
#include "etradeclient/mfc_ui/LoginDialog.h"

#include "etradeclient/browser/popup_browser_handler.h"
#include "etradeclient/utility/session.h"
#include "etradeclient/utility/logon_mgr.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/win_msg_define.h"

BEGIN_MESSAGE_MAP(CPopupBrowserView, CDialog)
	// System message.
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()

	// Customized CEF message handlers.
	ON_MESSAGE(WM_CEF_MSG_VIEW_NEW_BROWSER, &CPopupBrowserView::OnAttachCEFBrowser)
	ON_MESSAGE(WM_CEF_MSG_VIEW_CLOSE_BROWSER, &CPopupBrowserView::OnDetachCEFBrowser)
	ON_MESSAGE(WM_CEF_SESSION_EXPIRED, &CPopupBrowserView::OnSessionExpired)
	ON_MESSAGE(WM_CEF_TITLE_CHANGE, &CPopupBrowserView::OnTitleChange)
	ON_MESSAGE(WM_CEF_LOAD_END,&CPopupBrowserView::OnLoadEnd)
END_MESSAGE_MAP()

CPopupBrowserView::CPopupBrowserView(const RECT& main_wnd_rect, CWnd* pParent /*= NULL*/)
	: CDialog(CPopupBrowserView::IDD, pParent), m_rect(main_wnd_rect), m_browser_handler(nullptr)
{}

CPopupBrowserView::~CPopupBrowserView()
{}

int CPopupBrowserView::OnCreate(LPCREATESTRUCT create_struct)
{
	if (CDialog::OnCreate(create_struct) == -1)
		return -1;

	m_browser_handler = new PopupBrowserHandler();

	return 0;
}

BOOL CPopupBrowserView::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	MoveWindow(&m_rect, FALSE);

	// MoveWindow 需在CreateBrowser之前，避免创建的浏览器根据默认的窗口RECT来显示页面。
	CRect browser_rect;
	GetClientRect(browser_rect);
	CefBrowserSettings settings;
	CefWindowInfo info;
	info.SetAsChild(GetSafeHwnd(), browser_rect);
	if (!CefBrowserHost::CreateBrowser(info, m_browser_handler, URL(), settings, NULL))
	{
		LOG_ERROR(L"创建通知消息浏览器失败！");
		return FALSE;
	}
	return TRUE;
}

void CPopupBrowserView::OnSize(UINT type, int cx, int cy)
{
	CDialog::OnSize(type, cx, cy);

	if (!m_browser.CEFBrowserAttached())
		return;
	RECT rect;
	GetClientRect(&rect);
	m_browser.Resize(rect);
}

LRESULT CPopupBrowserView::OnAttachCEFBrowser(WPARAM wParam, LPARAM lParam)
{
	m_browser.AttachCEFBrowser((CefBrowser*)wParam);
	return 0;
}

LRESULT CPopupBrowserView::OnDetachCEFBrowser(WPARAM wParam, LPARAM lParam)
{
	m_browser.DetachCEFBrowser((CefBrowser*)wParam);
	SendMessage(WM_CLOSE);
	return 0;
}

LRESULT CPopupBrowserView::OnSessionExpired(WPARAM wParam, LPARAM lParam)
{
	this->ShowWindow(SW_HIDE); // Hide window first since we nee to show the login dialog.
	// AfxGetMainWnd() will return NULL if called from an other thread (has to do with thread local storage).
	// That's why we use AfxGetApp()->GetMainWnd() instead of AfxGetMainWnd().
	CWnd* main_frm = AfxGetApp()->GetMainWnd();
	if (!CLoginDialog(L"当前连接已过期，请重新登录！", main_frm).Launch())
	{
		::PostMessage(main_frm->GetSafeHwnd(), WM_CLOSE, NULL, NULL);
		SendMessage(WM_CLOSE);
		return 0;
	}
	Session::Instance().OnValid(); // If login succeeded, recover the session state to be "not expired".
	m_browser.Reload();
	this->ShowWindow(SW_SHOW); // Show window again after relogged in.
	return 0;
}

LRESULT CPopupBrowserView::OnLoadEnd(WPARAM wParam, LPARAM lParam)
{
	m_browser.SetFocus(TRUE);
	return 0;
}

LRESULT CPopupBrowserView::OnTitleChange(WPARAM wParam, LPARAM lParam)
{
	SetWindowText((LPCTSTR)wParam);
	return S_OK;
}

void CPopupBrowserView::OnClose()
{
	if (m_browser.CEFBrowserAttached())
	{
		m_browser.Close();
		return;// Browser会再次触发该接口
	}
	CDialog::OnClose();
}

// -----------------------------------------------
CUserMsgView::CUserMsgView(const RECT& main_wnd_rect, CWnd* pParent /*= NULL*/) : CPopupBrowserView(main_wnd_rect,pParent)
{
	const int kWidth = 240, kHorizontalMargin = 16, kHeight = 220, kVerticalMargin = 39, kToolbarHeight = 40;
	m_rect.left = m_rect.right - kWidth - kHorizontalMargin;
	m_rect.top += kToolbarHeight;
	m_rect.bottom = m_rect.top + kHeight + kVerticalMargin;
}
std::string CUserMsgView::URL() const
{
	auto& url_cfg = URLConfig::Instance();
	return url_cfg.FullHost() + url_cfg.UserMsgPath();
}

// -----------------------------------------------
CModifyPwdView::CModifyPwdView(const RECT& main_wnd_rect, CWnd* pParent /*= NULL*/) : CPopupBrowserView(main_wnd_rect, pParent)
{
	const int kWndWidth = 770, kHorizontalMargin = 20, kWndHeight = 350, kVerticalMargin = 40;
	const int kCenterX = m_rect.left + (m_rect.right - m_rect.left) / 2, kCenterY = m_rect.top + (m_rect.bottom - m_rect.top) / 2;
	m_rect.left = kCenterX - (kWndWidth + kHorizontalMargin) / 2;
	m_rect.right = kCenterX + (kWndWidth + kHorizontalMargin) / 2;
	m_rect.top = kCenterY - (kWndHeight + kVerticalMargin) / 2;
	m_rect.bottom = kCenterY + (kWndHeight + kVerticalMargin) / 2;
}
std::string CModifyPwdView::URL() const
{
	auto& url_cfg = URLConfig::Instance();
	return url_cfg.FullHost() + url_cfg.PwdModificationPath();
}

CCreateMerchantView::CCreateMerchantView(const RECT& main_wnd_rect, CWnd* pParent /*= NULL*/) : CPopupBrowserView(main_wnd_rect, pParent)
{
	const int kWndWidth = 770, kHorizontalMargin = 20, kWndHeight = 350, kVerticalMargin = 40;
	const int kCenterX = m_rect.left + (m_rect.right - m_rect.left) / 2, kCenterY = m_rect.top + (m_rect.bottom - m_rect.top) / 2;
	m_rect.left = kCenterX - (kWndWidth + kHorizontalMargin) / 2;
	m_rect.right = kCenterX + (kWndWidth + kHorizontalMargin) / 2;
	m_rect.top = kCenterY - (kWndHeight + kVerticalMargin) / 2;
	m_rect.bottom = kCenterY + (kWndHeight + kVerticalMargin) / 2;
}

std::string CCreateMerchantView::URL() const
{
#if 1
	auto& url_cfg = URLConfig::Instance();
	return url_cfg.FullHost() + url_cfg.CreateMerchantPath();
#else
	return "D:\\ApplicationWorkspace\\VisualStudio2013\\ETtradeClient\\hardware_test_html\\async_js_callbak_handler_test.html";
#endif

}
BEGIN_MESSAGE_MAP(CCreateMerchantView, CPopupBrowserView)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CCreateMerchantView::OnClose()
{
	CPopupBrowserView::OnClose();

	if (!LogonMgr::Instance().DoLogout())
		LOG_ERROR(L"创建商户操作完成后，服务端退出请求处理失败。");
	else
		LOG_TRACE(L"退出系统成功。");

	Session::Instance().OnExpired();// 不让主窗口关闭的时候，再去提示用户是否退出以及做退出后台系统处理。
	
	// AfxGetMainWnd() will return NULL if called from an other thread (has to do with thread local storage).
	// That's why we use AfxGetApp()->GetMainWnd() instead of AfxGetMainWnd().
	HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	::PostMessage(hwnd, WM_CLOSE, NULL, NULL);
}
