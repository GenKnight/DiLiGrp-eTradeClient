#pragma once

#include "afxwin.h"

#include <string>

class CLoginEdit : public CEdit
{
public:
	CLoginEdit();
	void UpdateStyle();
	virtual void GetContent(CString& content) const = 0;

private:
	virtual CString GetInputFieldName() const = 0;

private:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnKillfocus();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
	
protected:
	bool		m_is_empty;

private:
	CRect		m_top_fill_rect;
	CRect		m_bottom_fill_rect;
	CRect		m_left_fill_rect;

	COLORREF	m_color_text;
};

class CAccountEdit :public CLoginEdit
{
public:
	void SetContent(const std::wstring& content);
	void GetContent(CString& content) const;

private:
	CString GetInputFieldName() const;
};

class CPwdEdit : public CLoginEdit
{
public:
	void GetContent(CString& content) const;

private:
	afx_msg void OnEnChange();

	DECLARE_MESSAGE_MAP()

private:
	void SetMaskStr(unsigned int length);
	CString GetInputFieldName() const;

private:
	CString m_pwd;
};