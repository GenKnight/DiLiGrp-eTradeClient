#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxbutton.h"

#include "LoginEdit.h"

class CLoginDialog : public CDialogEx
{
	// LoginBtn
	class LoginBtn : public CButton
	{
	private:
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void DrawFilledRect(CDC* dc, CRect rc, COLORREF color);
		void DrawButtonText(CDC* dc, CRect rc, CString caption, COLORREF textcolor);
	};
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(const std::wstring& title = L"", CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDialog();
	bool Launch();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnLoginBtnClicked();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void SetSubCtrlLayout();
	void SetSubCtrlStyle();
	void AdjustDlgSize();
	BOOL LoadBackgroundImage();

private:
	enum { IDD = IDD_LOGIN };

	std::wstring		m_title;

	CStatic				m_login_err_msg_text;
	CAccountEdit		m_account_edit;
	CPwdEdit			m_pwd_edit;
	LoginBtn			m_login_btn;
	CImage				m_bg_img;

	CFont				m_btn_font;
	CFont				m_input_font;
};
