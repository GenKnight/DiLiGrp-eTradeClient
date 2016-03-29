
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
};

extern CETradeClientApp theApp;
