
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "MainFrm.h"

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <exception>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "etradeclient/boost_patch/property_tree/json_parser.hpp" // WARNIING! Make sure to include our patched version.
#include "etradeclient/mfc_ui/ETradeClient.h"
#include "etradeclient/mfc_ui/LoginDialog.h"
#include "etradeclient/mfc_ui/PopupBrowserDlgView.h"
#include "etradeclient/mfc_ui/ConfigDialog.h"

#include "etradeclient/browser/session.h"
#include "etradeclient/browser/browser_util.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/application_config.h"
#include "etradeclient/utility/win_msg_define.h"
#include "etradeclient/utility/url_config.h"
#include "etradeclient/utility/string_converter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fs = boost::filesystem;

namespace
{
	static const uint32_t MENU_ICON_WIDTH = 32, MENU_ICON_HEIGHT = 32;
	static const uint32_t TOOLBAR_ICON_WIDTH = 32, TOOLBAR_ICON_HEIGHT = 32;

	static const std::string JSON_TAG_CODE = "code";
	static const std::string JSON_TAG_MSG = "message";
	static const std::string JSON_VAL_SUCC = "success";

	enum ToolBarBtnID // Cannot use enum class due to TBBUTTON asks for integer.
	{
		RELOAD			= 4000,
		USER_MESSAGE	= 4001,
		USER_ACCOUNT	= 4002,
	};

	// Use relative path.
	static const std::wstring TOOLBAR_ICON_FOLDER(L".\\Resource\\Toolbar\\"); // Use 'wstring' because MFC ask for this.
	static const std::wstring MENU_ICON_FOLDER = L".\\Resource\\Menu\\";

	static std::vector<TBBUTTON> kQuickAccessBtn =
	{
		{ 0, ID_ISSUE_MASTER_CARD, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 },
		{ 1, ID_ISSUE_ANONYMOUS_CARD, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 },
		{ 2, ID_CASH_RECHARGE, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 },
		{ 3, ID_CASH_WITHDRAW, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 },
		{ 4, ID_SETTLE_ACCOUNTS_APPLY, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 }
	};
} // namespace

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_INITMENUPOPUP()

	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_MODIFY_PASSWORD, OnModifyPwd)
	ON_COMMAND(ID_MODIFY_CONFIG, OnModifyConfig)
	ON_UPDATE_COMMAND_UI(USER_MESSAGE, &CMainFrame::OnUpdateToolBarMsgCount)

	ON_COMMAND_RANGE(ID_ISSUE_MASTER_CARD, ID_OPER_LOG_QUERY, OnMenuBtnClicked)
	ON_COMMAND_RANGE(RELOAD, USER_ACCOUNT, OnToolBarBtnClicked)
	
	ON_MESSAGE(WM_CEF_SESSION_EXPIRED, OnSessionExpired)
	ON_MESSAGE(WM_UPDATE_USER_MSG_COUNT,OnGotMsgCount)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_is_view_closing(false), m_msg_count(0)
{}

CMainFrame::~CMainFrame()
{}

bool CMainFrame::Launch()
{
	auto& url_cfg = URLConfig::Instance();
	if (m_view.CreateBrowser(url_cfg.FullHost() + url_cfg.MainPath()))
	{
		m_user_msg_monitor.Start(); // Start the user msg monitor to monitor user msg update.
		return true;
	}
	LOG_ERROR(L"创建浏览器失败！");// Create main browser failed!

	if (!LogOut())
		LOG_ERROR(L"服务端退出请求处理失败。");
	else
		LOG_TRACE(L"退出系统成功。");

	return false;
}

void CMainFrame::UpdateStatus(LPCTSTR status)
{
	m_status_bar.SetPaneText(0, status);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_view.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	do
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			break;
		if (!m_menu_res_auth_manager.UpdateAuth())
			break;
		if (!FilterMenuBar())
			break;
		if (!FilterQuickAccessToolBar())
			break;
		if (!CreateMainFrmUI())
			break;
		return 0; // Return 0 if all succeed.
	} while (0);
	return -1; // Return -1 to indicate error happens.
}

void CMainFrame::OnClose()
{
	if (!m_is_view_closing)
	{
		if ( !Session::Instance().IsExpired() ) // Only prompt notification in logged in state.
		{
			if (IDNO == MessageBox(L"您确定要退出并关闭程序吗？", L"退出系统提示！", MB_YESNO | MB_ICONEXCLAMATION))
				return;
			if (!LogOut())
			{
				MessageBox(L"退出系统失败，若无法重新登录请联系维护人员！", L"退出系统提示！", MB_OK | MB_ICONWARNING);
				LOG_ERROR(L"服务端退出请求处理失败。程序关闭。");
			}
			else
				LOG_TRACE(L"退出系统成功。");
		}
		m_view.SendMessage(WM_CLOSE); // Request CETradeClientView to close.
		m_is_view_closing = true; // Enable next time close.
		return; // And cancel this close.
	}
	m_user_msg_monitor.Stop();
	LOG_TRACE(L"程序关闭。");

	CFrameWnd::OnClose();
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	do
	{
		if ((lpMeasureItemStruct == NULL) || (lpMeasureItemStruct->CtlType != ODT_MENU))
			break; // Don't handle items other than menu.
		const uint32_t kIconWidth = 24, kIconHeight = 24; // This is the most suitable size for showing a icon.
		lpMeasureItemStruct->itemWidth = kIconWidth;
		lpMeasureItemStruct->itemHeight = kIconHeight;
	} while (0);
	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	static const int32_t OFFSET_LEFT = -6, OFFSET_TOP = -4; // This is the most suitable size.
	do
	{
		if ((lpDrawItemStruct == NULL) || (lpDrawItemStruct->CtlType != ODT_MENU))
			break; // Don't handle items other than menu.

		::DrawIconEx(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left + OFFSET_LEFT, lpDrawItemStruct->rcItem.top + OFFSET_TOP,
			m_authorized_menu_icons[lpDrawItemStruct->itemID], MENU_ICON_WIDTH, MENU_ICON_HEIGHT, 0, NULL, DI_NORMAL);
	} while (0);

	CFrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMainFrame::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);

	if (bSysMenu)
		pMenu = GetSystemMenu(FALSE); // Allows the application to access the Control menu for copying and modification.

	//@{ Set menu style.
	MENUINFO mnfo;
	mnfo.cbSize = sizeof(mnfo);
	mnfo.fMask = MIM_STYLE;
	mnfo.dwStyle = MNS_CHECKORBMP | MNS_AUTODISMISS;
	pMenu->SetMenuInfo(&mnfo);
	//@}

	//@{ Set menu item attributes.
	MENUITEMINFO minfo;
	minfo.cbSize = sizeof(minfo);
	// Warning!!! Currently we have only 2 menu levels: top menu & sub menu.
	// So this algorithm is not general enough to handle a menu tree.
	for (int pos = 0; pos < pMenu->GetMenuItemCount(); ++pos)
	{
		minfo.fMask = MIIM_FTYPE | MIIM_ID;
		pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

		if (minfo.fType == MFT_SEPARATOR) // Don't handle the separator.
			continue;

		if (!(minfo.fType & MFT_OWNERDRAW)) // If not a owner draw menu item.
		{
			minfo.fMask = MIIM_FTYPE | MIIM_BITMAP; // Indicates the members to be retrieved or set: the 'hbmpItem' member..
			// "HBMMENU_CALLBACK" indicates taht a bitmap that is drawn by the window that owns the menu. 
			// The application must process the WM_MEASUREITEM and WM_DRAWITEM messages.
			minfo.hbmpItem = HBMMENU_CALLBACK;
			minfo.fType = MFT_STRING; // Displays the menu item using a text string.
			pMenu->SetMenuItemInfo(pos, &minfo, TRUE);
		}
	}
	//@}
}

void CMainFrame::OnUpdateToolBarMsgCount(CCmdUI *pCmdUI)
{
	if (m_msg_count > 0)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnToolBarBtnClicked(UINT btn_id)
{
	switch (btn_id)
	{
	case RELOAD:
		m_view.Browser().Reload();
		break;
	case USER_MESSAGE:
		ShowUserMessageDlg();
		break;
	default:
		break;
	}
}

void CMainFrame::OnMenuBtnClicked(UINT btn_id)
{
	m_view.Browser().NavigateTo(MenuUrl(btn_id));
}

void CMainFrame::OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTOOLBAR tool_bar = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);

	if (USER_ACCOUNT == tool_bar->iItem)
	{
		CMenu menu;
		menu.LoadMenu(IDR_ACCOUNT);
		CMenu*popup_menu = menu.GetSubMenu(0);
		ASSERT(popup_menu);

		CRect rc;
		m_ex_func_tlb.SendMessage(TB_GETRECT, tool_bar->iItem, (LPARAM)&rc);
		m_ex_func_tlb.ClientToScreen(&rc);
		popup_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, this, &rc);
	}
}

void CMainFrame::OnExit()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

void CMainFrame::OnModifyPwd()
{
	m_user_msg_monitor.Stop(); // Stop monitoring before launching the password modification dialog.
	RECT rect;
	GetWindowRect(&rect);
	CModifyPwdView(rect).DoModal();
	m_user_msg_monitor.Start(); // Recover monitoring.
}

void CMainFrame::OnModifyConfig()
{
	m_user_msg_monitor.Stop(); // Stop monitoring before launching the password modification dialog.
	CConfigDialog().DoModal();
	m_user_msg_monitor.Start(); // Recover monitoring.
}

afx_msg LRESULT CMainFrame::OnSessionExpired(WPARAM wParam, LPARAM lParam)
{
	m_user_msg_monitor.Stop(); // First stop the user message monitoring.
	if (!CLoginDialog(L"当前连接已过期，请重新登录！", this).Launch())
	{
		OnExit();
		return 0;
	}
	Session::Instance().OnValid(); // If login succeeded, recover the session state to be "not expired".
	m_user_msg_monitor.Start(); // Start again after relogged in.
	return 0;
}

afx_msg LRESULT CMainFrame::OnGotMsgCount(WPARAM wParam, LPARAM lParam)
{
	m_msg_count = (int)wParam;
	return 0;
}

bool CMainFrame::FilterMenuBar()
{
	CWnd* main_wnd = AfxGetApp()->m_pMainWnd;
	CMenu* menu = main_wnd->GetMenu();

	// Remove unauthorized menu.
	// !!! Through reverse traversal, guarantee the index order will not disorderly when remove next menu item.
	MenuResAuthMgr::MenuItemsType::const_reverse_iterator criter;
	const auto& menu_items = m_menu_res_auth_manager.MenuItems();

	std::vector<uint32_t>::const_iterator menu_index_iter;
	for (criter = menu_items.crbegin(); criter != menu_items.crend(); ++criter)
	{
		if (!criter->second.is_authorized) // Remove those unauthorized menu items.
		{
			CMenu* sub_menu = menu;
			// Traverse to the parent menu of the leaf node menu(which has no more submenu).
			const auto& index_list = criter->second.index_list;
			if (index_list.empty())
				break;
			// " -1 " means only traverse to the leaf node's parent node. Then the iter will be directed to the lead node.
			// , then we can call "RemoveMenu(*menu_index_iter, MF_BYPOSITION)" to remove the leaf node.
			// This works the same to the top level menu.
			for (menu_index_iter = index_list.cbegin(); menu_index_iter != index_list.cend() - 1; ++menu_index_iter)
			{
				sub_menu = sub_menu->GetSubMenu(*menu_index_iter);
				if (NULL == sub_menu)
				{
					std::stringstream err_msg;
					err_msg << "菜单配置错误, menu resource id: " << criter->first << "; menu index: " << *menu_index_iter << "!";
					LOG_ERROR(str_2_wstr(err_msg.str()));
					break;
				}
			}
			// Here we only need to remove the sub menu, since web server will not return an empty parent menu which has none sub menu.
			if (NULL == sub_menu || !sub_menu->RemoveMenu(*menu_index_iter, MF_BYPOSITION))
				LOG_ERROR(L"移除未授权菜单失败, menu resource id = " + std::to_wstring(criter->first) + L"!");
		}
		else
		{
			std::wstring file_path = MENU_ICON_FOLDER + str_2_wstr(criter->second.icon_name);
			if (!fs::exists(file_path))
			{
				LOG_ERROR(L"图片文件：" + file_path + L"不存在, 请检查！");
				break;
			}
			HICON hicon = (HICON)LoadImage(AfxGetResourceHandle(),
				file_path.c_str(), IMAGE_ICON, MENU_ICON_WIDTH, MENU_ICON_HEIGHT, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
			m_authorized_menu_icons.emplace(std::make_pair(criter->first, hicon));
			
			// 针对需要服务器端授权的菜单项，使用从服务器获取的菜单标题来更新菜单标题,
			// 对于无需服务器端授权的“本地菜单项”，则无需更新。
			if (!m_menu_res_auth_manager.IsLocalMenuItem(criter->first))
				menu->ModifyMenu(criter->first, MF_BYCOMMAND, criter->first, criter->second.remark.c_str());
		}
	}
	main_wnd->DrawMenuBar();
	return true;
}

bool CMainFrame::FilterQuickAccessToolBar()
{
	typedef MenuResAuthMgr::MenuItemsType MenuItemsT;
	const auto& menu_items = m_menu_res_auth_manager.MenuItems();
	for (auto iter = kQuickAccessBtn.begin(); iter != kQuickAccessBtn.end();)
	{
		MenuItemsT::const_iterator cit = menu_items.find(iter->idCommand);
		if (cit != menu_items.cend() && cit->second.is_authorized) // If found and authorized.
			++iter;
		else // If not found or unauthorized.
			iter = kQuickAccessBtn.erase(iter);
	}
	return true;
}

bool CMainFrame::CreateMainFrmUI()
{
	do
	{
		HICON frame_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		// Specifies a 32 pixel by 32 pixel icon if TRUE; specifies a 16 pixel by 16 pixel icon if FALSE.
		SetIcon(frame_icon, TRUE); // Set big icon
		SetIcon(frame_icon, FALSE); // Set small icon
		// create a view to occupy the client area of the frame
		if (!m_view.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
		{
			LOG_ERROR(L"Failed to create view window\n");
			break;
		}

		if (!m_status_bar.Create(this))
		{
			LOG_ERROR(L"Failed to create status bar\n");
			break;      // fail to create
		}
		m_status_bar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

		//@{
		if (!CreateWebNaviToolBar())
		{
			LOG_ERROR(L"Failed to create web navigation toolbar\n");
			break;      // fail to create
		}

		if (!CreateQuickAccessToolBar())
		{
			LOG_ERROR(L"Failed to create quick access toolbar\n");
			break;      // fail to create
		}

		if (!CreateExFuncToolBar())
		{
			LOG_ERROR(L"Failed to create extra function toolbar\n");
			break;      // fail to create
		}

		if (!m_re_bar.Create(this) ||
			!m_re_bar.AddBar(&m_web_navi_tlb, NULL, NULL, RBBS_NOGRIPPER) ||
			!m_re_bar.AddBar(&m_quick_access_tlb, NULL, NULL, RBBS_NOGRIPPER) ||
			!m_re_bar.AddBar(&m_ex_func_tlb, NULL, NULL, RBBS_NOGRIPPER))
		{
			LOG_ERROR(L"Failed to create rebar\n");
			break;      // fail to create
		}
		//@}

		m_re_bar.GetReBarCtrl().MinimizeBand(0);
		return true;
	} while (0);

	return false;
}

bool CMainFrame::CreateWebNaviToolBar()
{
	static const std::vector<std::wstring> kWebNaviIcon = { _T("reload.ico") };
	static std::vector<TBBUTTON> kWebNaviBtn = { { 0, RELOAD, TBSTATE_ENABLED, BTNS_BUTTON, 0, 0 } };

	if (!m_web_navi_tlb.CreateEx(this, TBSTYLE_TRANSPARENT))
		return false; // fail to create

	if (!m_web_navi_tlb_imgs.Create(TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, ILC_COLOR32 | ILC_MASK, 0, 0))
		return false;

	for (const auto& icon_name : kWebNaviIcon)
	{
		std::wstring file_path = TOOLBAR_ICON_FOLDER + icon_name;
		if (!fs::exists(file_path))
		{
			LOG_ERROR(L"图片文件：" + file_path + L"不存在, 请检查！");
			return false;
		}
		HICON icon = (HICON)LoadImage(
			AfxGetResourceHandle(), file_path.c_str(), IMAGE_ICON, TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		m_web_navi_tlb_imgs.Add(icon);
	}

	CToolBarCtrl& tbc = m_web_navi_tlb.GetToolBarCtrl();
	for (auto& btn : kWebNaviBtn)
		tbc.AddButtons(1, &btn);

	tbc.SetImageList(&m_web_navi_tlb_imgs);

	const uint32_t kPaddingWidth = 45;
	UpdateToolbarBtnSize(m_web_navi_tlb, kPaddingWidth); // Make sure to set toolbar button size after the button text has been set.
	
	return true;
}

bool CMainFrame::CreateQuickAccessToolBar()
{
	if (!m_quick_access_tlb.CreateEx(this, TBSTYLE_TRANSPARENT | TBSTYLE_LIST))
		return false; // fail to create
	if (!m_quick_access_tlb_imgs.Create(TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, ILC_COLOR32 | ILC_MASK, 0, 0))
		return false;
	
	CToolBarCtrl& tbc = m_quick_access_tlb.GetToolBarCtrl();
	tbc.SetImageList(&m_quick_access_tlb_imgs); // 注意这一句必须放在为“SetButtonText”和“UpdateToolbarBtnSize”之前，否则会出现宽度计算错误

	const auto& res_auth_map = m_menu_res_auth_manager.MenuItems();
	int index = 0;
	for (auto& btn : kQuickAccessBtn)
	{
		tbc.AddButtons(1, &btn);

		std::wstring file_path = TOOLBAR_ICON_FOLDER + str_2_wstr(res_auth_map.at(btn.idCommand).icon_name);
		if (!fs::exists(file_path))
		{
			LOG_ERROR(L"图片文件：" + file_path + L"不存在, 请检查！");
			return false;
		}
		HICON icon = (HICON)LoadImage(
			AfxGetResourceHandle(), file_path.c_str(), IMAGE_ICON, TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		m_quick_access_tlb_imgs.Add(icon);
		m_quick_access_tlb.SetButtonText(index, res_auth_map.at(btn.idCommand).remark.c_str());
		index++;
	}
	
	UpdateToolbarBtnSize(m_quick_access_tlb); // Make sure to set toolbar button size after the button text has been set.
	return true;
}

bool CMainFrame::CreateExFuncToolBar()
{
	static std::vector<std::wstring> kExFuncText =
	{
		_T("消息"),
		_T("账户")
	};
	static const std::vector<std::wstring> kExFuncIcon =
	{
		_T("message.ico"),
		_T("user.ico")
	};
	// Icon for the disabled toolbar buttons.
	static const std::vector<std::wstring> kExFuncIconDisabled =
	{
		_T("message_d.ico"),
		_T("user_d.ico")
	};
	static std::vector<TBBUTTON> kExFuncBtn =
	{
		{ 0, USER_MESSAGE, TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_BUTTON, 0, 0 },
		{ 1, USER_ACCOUNT, TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_BUTTON | BTNS_WHOLEDROPDOWN, 0, 0 }
	};

	if (!m_ex_func_tlb.CreateEx(this, TBSTYLE_TRANSPARENT | TBSTYLE_LIST))
		return false; // fail to create
	if (!m_ex_func_tlb_imgs.Create(TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, ILC_COLOR32 | ILC_MASK, 0, 0) ||
		!m_ex_func_tlb_disabled_imgs.Create(TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, ILC_COLOR32 | ILC_MASK, 0, 0))
		return false;

	for (const auto& icon_name : kExFuncIcon)
	{
		std::wstring file_path = TOOLBAR_ICON_FOLDER + icon_name;
		if (!fs::exists(file_path))
		{
			LOG_ERROR(L"图片文件：" + file_path + L"不存在, 请检查！");
			return false;
		}
		HICON icon = (HICON)LoadImage(
			AfxGetResourceHandle(), file_path.c_str(), IMAGE_ICON, TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		m_ex_func_tlb_imgs.Add(icon);
	}

	for (const auto& icon_name : kExFuncIconDisabled)
	{
		std::wstring file_path = TOOLBAR_ICON_FOLDER + icon_name;
		if (!fs::exists(file_path))
		{
			LOG_ERROR(L"图片文件：" + file_path + L"不存在, 请检查！");
			return false;
		}
		HICON icon = (HICON)LoadImage(
			AfxGetResourceHandle(), file_path.c_str(), IMAGE_ICON, TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		m_ex_func_tlb_disabled_imgs.Add(icon);
	}

	CToolBarCtrl& tbc = m_ex_func_tlb.GetToolBarCtrl();
	for (auto& btn : kExFuncBtn)
		tbc.AddButtons(1, &btn);

	tbc.SetImageList(&m_ex_func_tlb_imgs);
	tbc.SetDisabledImageList(&m_ex_func_tlb_disabled_imgs);

	int index = 0;
	for (auto& text : kExFuncText)
	{
		m_ex_func_tlb.SetButtonText(index, text.c_str());
		index++;
	}

	// 设置用户名
	const int kAccountBtnIndex = 1;
	m_ex_func_tlb.SetButtonText(kAccountBtnIndex, Session::Instance().UserName().c_str());
	UpdateToolbarBtnSize(m_ex_func_tlb); // Make sure to set toolbar button size after the button text has been set.
	return true;
}

void CMainFrame::UpdateToolbarBtnSize(CToolBar& toolbar, uint32_t padding_width, uint32_t padding_height)
{
	CRect btn_rect;
	int total_btns_width = 0;
	int btns_count = toolbar.GetToolBarCtrl().GetButtonCount();
	if (btns_count<=0)
		return;
	for (int idx = 0; idx < btns_count; ++idx)
	{
		toolbar.GetItemRect(idx, &btn_rect);
		total_btns_width += btn_rect.Size().cx;
	}
	const int btn_size_x = total_btns_width / btns_count;// 计算按钮宽度平均值
	const int btn_size_y = btn_rect.Size().cy;
	toolbar.SetSizes(CSize(btn_size_x + padding_width, btn_size_y + padding_height), CSize(TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT));
}

bool CMainFrame::LogOut()
{
	bool log_out_res = true;
	std::string response_body;
	try
	{
		response_body = WinHttpGet(URLConfig::Instance().LogoutPath());
	}
	catch (std::exception& ex)
	{
		LOG_ERROR(L"网络连接有问题，退出系统失败。错误信息： " + gbk_2_wstr(ex.what()));
		return false;
	}

	namespace PT = boost::property_tree;
	try //Parse the configuration file
	{
		PT::ptree ptree;
		std::stringstream ss;
		ss << response_body;
		PT::read_json(ss, ptree);
		std::string res = ptree.get<std::string>(JSON_TAG_CODE);
		log_out_res = res.compare(JSON_VAL_SUCC) == 0 ? true : false;
		if (!log_out_res)
			LOG_ERROR(L"服务器退出处理失败，返回结果为：" + str_2_wstr(ptree.get<std::string>(JSON_TAG_MSG)));
	}
	catch (...)
	{
		LOG_ERROR(L"解析服务器返回的退出信息时出错！请确认返回数据不为空，返回的数据格式为正确的Json格式！");
		return false;
	}
	return log_out_res;
}

void CMainFrame::ShowUserMessageDlg()
{
	m_user_msg_monitor.Stop(); // Stop monitoring before launching the msg view dialog.
	RECT rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	CUserMsgView(rect).DoModal();
	m_user_msg_monitor.Start(); // Recover monitoring.
}

std::string CMainFrame::MenuUrl(uint32_t menu_res_id) const
{
	auto& url_cfg = URLConfig::Instance();
	return url_cfg.FullHost() + m_menu_res_auth_manager.MenuItems().at(menu_res_id).url_path;
}

std::string CMainFrame::WinHttpGet(const std::string& url_path) const
{
	const uint32_t kHTTPOK = 200;
	auto& url_cfg = URLConfig::Instance();

	WinHttp win_http;
	win_http.ConnectHost(url_cfg.Host(), url_cfg.Port(), url_cfg.IsHttps());
	auto& request = win_http.OpenRequest(WinHttp::Method::GET, url_path);
	if (url_cfg.IsHttps())
	{
		auto& app_cfg = ApplicationConfig::Instance();
		request.SetClientCertificate(app_cfg.ClientCertStore(), app_cfg.ClientCertSubject());
	}
	request.SetCookies(Session::Instance().Cookies());
	request.Send();
	uint32_t status_code = request.GetResponseStatus();
	if (kHTTPOK != status_code)
	{
		std::string err_msg = "网络请求错误！ 错误码: " + std::to_string(status_code);
		throw std::exception(err_msg.c_str());
	}
	std::string response_body = request.ReadResponseBody();
	if (response_body.empty())
		throw std::exception("获取服务器响应数据失败，请确保网络连接正常");
	return response_body;
}