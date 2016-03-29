//
#pragma once

#include <afxwin.h>

#include "Resource.h"

#include "include/internal/cef_ptr.h"

#include "etradeclient/browser/embedded_browser.h"

// Forward declaration.
class PopupBrowserHandler;

class CPopupBrowserView : public CDialog
{
public:
	explicit CPopupBrowserView(const RECT& main_wnd_rect, CWnd* pParent = NULL);
	virtual ~CPopupBrowserView();

private:
	virtual std::string URL() const = 0;

	enum { IDD = IDD_POPUP_BROWSER_VIEW };

	int OnCreate(LPCREATESTRUCT create_struct);
	BOOL OnInitDialog();
	void OnSize(UINT type, int cx, int cy);

	afx_msg LRESULT OnAttachCEFBrowser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDetachCEFBrowser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSessionExpired(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTitleChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadEnd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
protected:
	RECT					m_rect;
private:
	CefRefPtr<PopupBrowserHandler>	m_browser_handler;
	EmbeddedBrower					m_browser;
};

class CUserMsgView : public CPopupBrowserView
{
public:
	explicit CUserMsgView(const RECT& main_wnd_rect, CWnd* pParent = NULL);
private:
	std::string URL() const;
};

class CModifyPwdView : public CPopupBrowserView
{
public:
	explicit CModifyPwdView(const RECT& main_wnd_rect, CWnd* pParent = NULL);
private:
	std::string URL() const;
};