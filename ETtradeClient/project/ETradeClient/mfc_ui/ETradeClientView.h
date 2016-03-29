
// ETradeClientView.h : interface of the CETradeClientView class
//
#pragma once

#include <string>
#include <cstdint>

#include "include/internal/cef_ptr.h"

#include "etradeclient/browser/embedded_browser.h"

class MainViewBrowserHandler; // Forward declaration.

class CETradeClientView : public CWnd
{
// Construction
public:
	CETradeClientView();
	~CETradeClientView();

	bool CreateBrowser(const std::string& url);
	const EmbeddedBrower& Browser() const;
	uint32_t BrowserState() const;

// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();

	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();

	// Customized CEF message handlers.
	afx_msg LRESULT OnBeforeBrowse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTitleChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddressChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatusMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewBrowser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloseBrowser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWindowCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAuthenticate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBadCertificate(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

private:
	bool IsEditCtrl(CWnd* pWnd); // Check whether current focused windows is edit ctrl.

private:
	EmbeddedBrower						m_browser;
	CefRefPtr<MainViewBrowserHandler>	m_browser_handler; // MainViewBrowserHandler implements browser-level callbacks.
	uint32_t							m_browser_state;
};

