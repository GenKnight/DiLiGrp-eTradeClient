
// ETradeClient.h : main header file for the ETradeClient application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include <string>

#include "include/internal/cef_ptr.h"
#include "include/internal/cef_string.h"

// CETradeClientApp:
// See ETradeClient.cpp for the implementation of this class
//

class BrowserApp; // Forward declaration.

class CETradeClientApp : public CWinApp
{
	// Instance manager to prevent multiple running instances.
	class InstanceManager
	{
	public:
		InstanceManager();
		~InstanceManager();
		bool Register(const std::wstring& app_id);
		void Unregister();
	private:
		HANDLE m_mutex_handle;
	};

public:
	CETradeClientApp();

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual BOOL PumpMessage();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	bool InitializeCef();

private:
	CefRefPtr<BrowserApp>	m_browser_app;
	bool					m_cef_initialized;
	InstanceManager			m_instance_mgr;
};

extern CETradeClientApp theApp;
