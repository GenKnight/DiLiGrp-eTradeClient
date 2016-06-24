#pragma once

#include "afxwin.h"

#include <string>

class CLoginEdit : public CEdit
{
public:
	explicit CLoginEdit(const CString& default_text = L"", bool use_pwd_char = false);

	void SetDefaultText(const CString& default_text);
	void SetText(const CString& content);
	CString GetText() const;
private:
	virtual void PreSubclassWindow();
	void UpdateStyle();

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnKillfocus();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

private:
	CRect		m_top_fill_rect;
	CRect		m_bottom_fill_rect;
	CRect		m_left_fill_rect;

	COLORREF	m_color_text;

	bool		m_use_pwd_char;
	CString		m_default_text;
};
