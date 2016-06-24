
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ETradeClientView.h"

#include <string>
#include <map>
#include <cstdint>

#include "etradeclient/browser/user_msg_monitor.h"
#include "etradeclient/utility/menu_res_auth_mgr.h"

class CMainFrame : public CFrameWnd
{
	typedef std::map<uint32_t, HICON> MenuIconsType;
public:
	CMainFrame();
	virtual ~CMainFrame();

	bool Launch(); // Lauch the browser window.
	void UpdateStatus(LPCTSTR status);

	void DoCreatMerchant();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_DYNAMIC(CMainFrame)

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();

	// Handlers for drawing the icon of the menu item.
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateToolBarMsgCount(CCmdUI *pCmdUI);
	afx_msg void OnToolBarBtnClicked(UINT btn_id);
	afx_msg void OnMenuBtnClicked(UINT btn_id);
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExit();
	afx_msg void OnModifyPwd();
	afx_msg void OnModifyConfig();

	// Customized message handlers.
	afx_msg LRESULT OnSessionExpired(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGotMsgCount(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	bool FilterMenuBar();
	bool FilterQuickAccessToolBar();
	bool CreateMainFrmUI();
	bool CreateWebNaviToolBar();
	bool CreateQuickAccessToolBar();
	bool CreateExFuncToolBar();
	void UpdateToolbarBtnSize(CToolBar& toolbar, uint32_t padding_width = 0, uint32_t padding_height = 0);
	void ShowUserMessageDlg();
	std::string MenuUrl(uint32_t menu_res_id) const;

private:
	CStatusBar 			m_status_bar;
	CETradeClientView 	m_view;

	CToolBar 			m_web_navi_tlb;
	CImageList 			m_web_navi_tlb_imgs;

	CToolBar			m_quick_access_tlb;
	CImageList			m_quick_access_tlb_imgs;

	CToolBar 			m_ex_func_tlb; // tool bar to provide extra functional.
	CImageList 			m_ex_func_tlb_imgs;
	CImageList 			m_ex_func_tlb_disabled_imgs;
	CReBar 				m_re_bar;

	MenuResAuthMgr		m_menu_res_auth_manager;
	MenuIconsType		m_authorized_menu_icons;

	UserMsgMonitor		m_user_msg_monitor;
	int					m_msg_count;
	
	bool 				m_is_view_closing;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};


