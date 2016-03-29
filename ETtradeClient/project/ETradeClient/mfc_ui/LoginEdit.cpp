#include "stdafx.h"

#include "LoginEdit.h"

BEGIN_MESSAGE_MAP(CLoginEdit, CEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CLoginEdit::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CLoginEdit::OnEnKillfocus)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

CLoginEdit::CLoginEdit()
	: m_bottom_fill_rect(0, 0, 0, 0), m_top_fill_rect(0, 0, 0, 0),
	  m_is_empty(true), m_color_text(GetSysColor(COLOR_WINDOWTEXT))
{}

void CLoginEdit::UpdateStyle()
{
	m_color_text = GetSysColor(COLOR_WINDOWTEXT);
	if (!m_is_empty)
		return;
	if (GetSafeHwnd() == ::GetFocus())
		SetWindowText(L"");
	else
	{
		m_color_text = RGB(120, 120, 120);
		SetWindowText(GetInputFieldName());
	}
}

void CLoginEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	CRect wnd_rect, client_rect, text_rect;

	//calculate client area height needed for a font
	CDC* dc = GetDC();
	CFont* old_font = dc->SelectObject(GetFont());
	text_rect.SetRectEmpty();
	dc->DrawText(L"Default", text_rect, DT_CALCRECT | DT_LEFT);//获取文本区大小
	UINT text_area_height = text_rect.Height();
	dc->SelectObject(old_font);
	ReleaseDC(dc);

	GetClientRect(client_rect);
	GetWindowRect(wnd_rect);

	const int center_offset = (client_rect.Height() - text_area_height) / 2;
	const int margin_y = (wnd_rect.Height() - client_rect.Height()) / 2;
	const int margin_x = (wnd_rect.Width() - client_rect.Width()) / 2;
	const int kInputMargin = 20;

	//计算文本区大小(计算为绝对值)
	auto& text_area_rect = lpncsp->rgrc[0];
	text_area_rect.top += center_offset;
	text_area_rect.left += margin_x + kInputMargin;
	text_area_rect.right -= margin_x;
	text_area_rect.bottom -= center_offset;

	// 计算上下填充区大小(计算为相对值)
	m_top_fill_rect.top = margin_y;
	m_top_fill_rect.left = margin_x;
	m_top_fill_rect.right = m_top_fill_rect.left + (text_area_rect.right - text_area_rect.left) + kInputMargin;
	m_top_fill_rect.bottom = m_top_fill_rect.top + center_offset;

	m_bottom_fill_rect.top = margin_y + center_offset + text_area_height;
	m_bottom_fill_rect.left = margin_x;
	m_bottom_fill_rect.right = m_bottom_fill_rect.left + (text_area_rect.right - text_area_rect.left) + kInputMargin;
	m_bottom_fill_rect.bottom = m_bottom_fill_rect.top + center_offset;

	m_left_fill_rect.top = m_top_fill_rect.bottom;
	m_left_fill_rect.left = margin_x;
	m_left_fill_rect.right = margin_x + kInputMargin;
	m_left_fill_rect.bottom = m_bottom_fill_rect.top;
} 

void CLoginEdit::OnNcPaint()
{
	Default();//绘制默认样式

	CWindowDC dc(this);

	CBrush fill_brush(GetSysColor(COLOR_WINDOW));

	dc.FillRect(m_bottom_fill_rect, &fill_brush);
	dc.FillRect(m_top_fill_rect, &fill_brush);
	dc.FillRect(m_left_fill_rect, &fill_brush);
}

UINT CLoginEdit::OnGetDlgCode()
{
	if (m_top_fill_rect.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}

void CLoginEdit::OnEnSetfocus()
{
	if (m_is_empty)
	{
		SetWindowText(L"");
	}
	UpdateStyle();
}

void CLoginEdit::OnEnKillfocus()
{
	CString text_content;
	GetWindowText(text_content);
	if (!text_content.IsEmpty())
		m_is_empty = false;
	else
		m_is_empty = true;
	UpdateStyle();
}

HBRUSH CLoginEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_color_text);
	return CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

void CAccountEdit::SetContent(const std::wstring& content)
{
	m_is_empty = content.empty();
	SetWindowText(content.c_str());
}

void CAccountEdit::GetContent(CString& content) const
{
	if (!m_is_empty)
		GetWindowText(content);
	else
		content = L"";
}

CString CAccountEdit::GetInputFieldName() const
{
	return L"用户名";
}

BEGIN_MESSAGE_MAP(CPwdEdit, CLoginEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &CPwdEdit::OnEnChange)
END_MESSAGE_MAP()

void CPwdEdit::GetContent(CString& content) const
{
	if (!m_is_empty)
		content = m_pwd;
	else
		content = L"";
}

void CPwdEdit::OnEnChange()
{
	CString text_content;
	GetWindowText(text_content);

	const int pwd_length = m_pwd.GetLength();
	const int content_length = text_content.GetLength();
	const int d_value = content_length - pwd_length;

	if (GetSafeHwnd() == ::GetFocus() && 0 != d_value)
	{
		m_is_empty = content_length <= 0;

		if (d_value > 0)
			m_pwd += text_content.Mid(pwd_length);
		else
			m_pwd = m_pwd.Mid(0, content_length);

		SetMaskStr(content_length);
		SetSel(content_length, content_length);
	}
}

void CPwdEdit::SetMaskStr(unsigned int length)
{
	const wchar_t kMask = 0x25CF; // "●" 
	SetWindowText(std::wstring(length, kMask).c_str());
}

CString CPwdEdit::GetInputFieldName() const
{
	return L"密码";
}


