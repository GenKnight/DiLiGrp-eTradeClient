#include "stdafx.h"

#include "LoginEdit.h"

static const wchar_t PWD_CHAR = 0x25CF; // "●" 

BEGIN_MESSAGE_MAP(CLoginEdit, CEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CLoginEdit::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CLoginEdit::OnEnKillfocus)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

CLoginEdit::CLoginEdit(const CString& default_text, bool use_pwd_char) :
	m_use_pwd_char(use_pwd_char),
	m_default_text(default_text), 
	m_bottom_fill_rect(0, 0, 0, 0), 
	m_top_fill_rect(0, 0, 0, 0),
	m_color_text(GetSysColor(COLOR_WINDOWTEXT))
{}

void CLoginEdit::UpdateStyle()
{
	m_color_text = GetSysColor(COLOR_WINDOWTEXT);
	CString curr_text;
	GetWindowText(curr_text);
	if (!curr_text.IsEmpty() && curr_text != m_default_text)
		return;
	if (GetSafeHwnd() == ::GetFocus())
		SetWindowText(L"");
	else
	{
		m_color_text = RGB(120, 120, 120);
		SetWindowText(m_default_text);
	}
}

void CLoginEdit::SetText(const CString& content)
{
	SetWindowText(content);
}
CString CLoginEdit::GetText() const
{
	CString text;
	GetWindowText(text);
	return text == m_default_text ? L"" : text;
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
	if (m_use_pwd_char)
		SetPasswordChar(PWD_CHAR); // Use password mode.
	UpdateStyle();
}

void CLoginEdit::OnEnKillfocus()
{
	if (m_use_pwd_char)
	{
		CString curr_text;
		GetWindowText(curr_text);
		if (curr_text.IsEmpty())
			SetPasswordChar(0); // Turn off the password mode.
	}
	UpdateStyle();
}

HBRUSH CLoginEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_color_text);
	return CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}
