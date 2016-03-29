
// ETradeClientView.cpp : implementation of the CETradeClientView class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "ETradeClient.h"
#include "ETradeClientView.h"

#include "include/cef_browser.h"
#include "include/internal/cef_types_wrappers.h"

#include "etradeclient/browser/browser_util.h"
#include "etradeclient/browser/main_view_browser_handler.h"
#include "etradeclient/utility/win_msg_define.h"
#include "etradeclient/utility/logging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CETradeClientView, CWnd)

	// System message.
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()

	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CETradeClientView::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, &CETradeClientView::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CETradeClientView::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, &CETradeClientView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CETradeClientView::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, &CETradeClientView::OnEditPaste)

	// Customized CEF message handlers.
	ON_MESSAGE(WM_CEF_LOAD_START, &CETradeClientView::OnLoadStart)
	ON_MESSAGE(WM_CEF_LOAD_END, &CETradeClientView::OnLoadEnd)
	ON_MESSAGE(WM_CEF_TITLE_CHANGE, &CETradeClientView::OnTitleChange)
	ON_MESSAGE(WM_CEF_STATE_CHANGE, &CETradeClientView::OnStateChange)
	ON_MESSAGE(WM_CEF_ADDRESS_CHANGE, &CETradeClientView::OnAddressChange)
	ON_MESSAGE(WM_CEF_STATUS_MESSAGE, &CETradeClientView::OnStatusMessage)
	ON_MESSAGE(WM_CEF_BEFORE_BROWSE, &CETradeClientView::OnBeforeBrowse)
	ON_MESSAGE(WM_CEF_DOWNLOAD_UPDATE, &CETradeClientView::OnDownloadUpdate)
	ON_MESSAGE(WM_CEF_LOAD_ERROR, &CETradeClientView::OnLoadError)
	ON_MESSAGE(WM_CEF_CLOSE_BROWSER, &CETradeClientView::OnCloseBrowser)
	ON_MESSAGE(WM_CEF_NEW_BROWSER, &CETradeClientView::OnNewBrowser)
	ON_MESSAGE(WM_CEF_WINDOW_CHECK, &CETradeClientView::OnWindowCheck)
	ON_MESSAGE(WM_CEF_AUTHENTICATE, &CETradeClientView::OnAuthenticate)
	ON_MESSAGE(WM_CEF_BAD_CERTIFICATE, &CETradeClientView::OnBadCertificate)	
END_MESSAGE_MAP()


// CETradeClientView
CETradeClientView::CETradeClientView() : m_browser_state(0)
{}

CETradeClientView::~CETradeClientView()
{}

bool CETradeClientView::CreateBrowser(const std::string& url)
{
	CRect rect;
	this->GetClientRect(rect); // get rect
	
	CefBrowserSettings settings; // Specify CEF browser settings here.	
	CefWindowInfo info; // Information used when creating the native window.
	info.SetAsChild(this->GetSafeHwnd(), rect); // set browser as child
	return CefBrowserHost::CreateBrowser(info, m_browser_handler.get(), url, settings, NULL); // create CEF Browser
}

const EmbeddedBrower& CETradeClientView::Browser() const
{
	return m_browser;
}

uint32_t CETradeClientView::BrowserState() const
{
	return m_browser_state;
}

// CETradeClientView message handlers

BOOL CETradeClientView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

int CETradeClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_browser_handler = new MainViewBrowserHandler(); // create client handler object after the CEF is initialized.
	return 0;
}

void CETradeClientView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_browser.CEFBrowserAttached())
	{
		RECT rect;
		GetClientRect(&rect);
		m_browser.Resize(rect);
	}
}

BOOL CETradeClientView::OnEraseBkgnd(CDC* pDC)
{
	if (m_browser.CEFBrowserAttached())
		return 0; // Dont erase the background if the browser window has been loaded (this avoids flashing)

	return CWnd::OnEraseBkgnd(pDC);
}

void CETradeClientView::OnClose()
{
	if (m_browser.CEFBrowserAttached())
	{
		m_browser.Close();
		return; // Cancel this time close.
	}

	CWnd::OnClose();
}

void CETradeClientView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
	{
		INT start = 0, end = 0;
		pEdit->GetSel(start, end);
		pCmdUI->Enable(start != end ? TRUE : FALSE);
	}
	else
	{
		if (m_browser.CEFBrowserAttached())
			pCmdUI->Enable(TRUE);
	}
}

void CETradeClientView::OnEditCut()
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
		pEdit->Copy();
	else if (m_browser.CEFBrowserAttached())
		m_browser.Cut();
}

void CETradeClientView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
	{
		INT start = 0, end = 0;
		pEdit->GetSel(start, end);
		pCmdUI->Enable(start != end ? TRUE : FALSE);
	}
	else
	{
		if (m_browser.CEFBrowserAttached())
			pCmdUI->Enable(TRUE);
	}
}

void CETradeClientView::OnEditCopy()
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
		pEdit->Copy();
	else if (m_browser.CEFBrowserAttached())
		m_browser.Copy();
}

void CETradeClientView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
	{
		INT nText = IsClipboardFormatAvailable(CF_TEXT);
		pCmdUI->Enable(nText ? TRUE : FALSE);
	}
	else
	{
		if (m_browser.CEFBrowserAttached())
			pCmdUI->Enable(TRUE);
	}
}

void CETradeClientView::OnEditPaste()
{
	CEdit* pEdit = (CEdit*)GetFocus();
	if (IsEditCtrl(pEdit))
		pEdit->Paste();
	else if (m_browser.CEFBrowserAttached())
		m_browser.Paste();
}

LRESULT CETradeClientView::OnBeforeBrowse(WPARAM wParam, LPARAM lParam)
{
	// get URL
	LPCTSTR url = (LPCTSTR)wParam;
	BOOL is_redirected = (BOOL)lParam;

	// passed the test so allow naviagtion
	return S_OK;
}

LRESULT CETradeClientView::OnDownloadUpdate(WPARAM wParam, LPARAM lParam)
{
	// get CEF info
	CEFDownloadItemValues* download_item = (CEFDownloadItemValues*)wParam;

	return S_OK;
}

LRESULT CETradeClientView::OnLoadStart(WPARAM wParam, LPARAM lParam)
{
	// size browser
	SendMessage(WM_SIZE);

	return S_OK;
}

LRESULT CETradeClientView::OnLoadEnd(WPARAM wParam, LPARAM lParam)
{
	static const int kHttpCode_OK = 200;
	static const int kFileCode_OK = 0; // code of loading local file
	// size browser
	SendMessage(WM_SIZE);

	CMainFrame* frm = dynamic_cast<CMainFrame*>(GetParentFrame());
	if (kHttpCode_OK == (int)wParam || kFileCode_OK == (int)wParam)
		frm->UpdateStatus(L"Ò³Ãæ¼ÓÔØ½áÊø£¡");
	else
		frm->UpdateStatus(L"Ò³Ãæ¼ÓÔØÊ§°Ü£¡");

	frm->SetFocus(); // Important!!! Let main frame get focus so that it can refresh the toolbar buttons when state changes.
	m_browser.SetFocus(true); // Then set the focus back to the browser window!!!

	return S_OK;
}

LRESULT CETradeClientView::OnTitleChange(WPARAM wParam, LPARAM lParam)
{
	GetParentFrame()->SetWindowTextW((LPCTSTR)wParam); // set title
	return S_OK;
}

LRESULT CETradeClientView::OnStateChange(WPARAM wParam, LPARAM lParam)
{
	m_browser_state = (INT)wParam; // Store the browser state.
	return S_OK;
}

LRESULT CETradeClientView::OnAddressChange(WPARAM wParam, LPARAM lParam)
{
	// get URL
	LPCTSTR url = (LPCTSTR)wParam;
	return S_OK;
}

LRESULT CETradeClientView::OnStatusMessage(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* frm = dynamic_cast<CMainFrame*>(GetParentFrame());
	frm->UpdateStatus((LPCTSTR)wParam);
	return S_OK;
}

LRESULT CETradeClientView::OnLoadError(WPARAM wParam, LPARAM lParam)
{
	LOG_ERROR(L"Ò³Ãæ¼ÓÔØÊ§°Ü£¬´íÎó´úÂë£º " + std::wstring((LPCTSTR)lParam));
	return S_OK;
}

LRESULT CETradeClientView::OnNewBrowser(WPARAM wParam, LPARAM lParam)
{
	m_browser.AttachCEFBrowser((CefBrowser*)lParam); // Hold reference to created browser.
	return S_OK;
}

LRESULT CETradeClientView::OnCloseBrowser(WPARAM wParam, LPARAM lParam)
{
	m_browser.DetachCEFBrowser((CefBrowser*)wParam); // Release reference of CEF browser instance.
	return S_OK;
}

LRESULT CETradeClientView::OnWindowCheck(WPARAM wParam, LPARAM lParam)
{
	// get CEF info
	CefPopupFeatures* popup_features = (CefPopupFeatures*)wParam;
	LPCTSTR url = (LPCTSTR)lParam;

	// good to go
	return S_OK;
}

LRESULT CETradeClientView::OnAuthenticate(WPARAM wParam, LPARAM lParam)
{
	// get name and realm
	CEFAuthenticationValues* lpValues = (CEFAuthenticationValues*)wParam;

	return S_FALSE;
}

LRESULT CETradeClientView::OnBadCertificate(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

bool CETradeClientView::IsEditCtrl(CWnd* pWnd)
{
	if (nullptr == pWnd)
		return false;

	HWND hWnd = pWnd->GetSafeHwnd();
	if (nullptr == hWnd)
		return false;

	TCHAR szClassName[6];
	::GetClassName(hWnd, szClassName, 6) && _tcsicmp(szClassName, _T("Edit")) == 0;
	//TRACE( _T("Class=%s\n"), szClassName );

	return ::GetClassName(hWnd, szClassName, 6) && _tcsicmp(szClassName, _T("Edit")) == 0;
}
