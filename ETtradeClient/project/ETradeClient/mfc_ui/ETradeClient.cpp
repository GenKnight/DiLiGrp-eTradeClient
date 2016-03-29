
// ETradeClient.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"

#include "ETradeClient.h"
#include "MainFrm.h"
#include "LoginDialog.h"

#include <boost/filesystem.hpp>

#include "etradeclient/browser/browser_app.h"
#include "etradeclient/browser/embedded_browser.h"
#include "etradeclient/utility/logging.h"
#include "etradeclient/utility/string_converter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

// Set to 0 to disable sandbox support.
#define CEF_ENABLE_SANDBOX 0
#if CEF_ENABLE_SANDBOX
// The cef_sandbox.lib static library is currently built with VS2010. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Replace application ID string below with unique ID string; recommended
// format for string is CompanyName.ProductName.SubProduct.VersionInformation
static const wchar_t* APP_ID = L"DILIGROUP.ETradeClient.V1.0";


// CETradeClientApp

BEGIN_MESSAGE_MAP(CETradeClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CETradeClientApp::OnAppAbout)
END_MESSAGE_MAP()

// CETradeClientApp construction

CETradeClientApp::CETradeClientApp() : m_browser_app(nullptr), m_cef_initialized(false)
{

	SetAppID(APP_ID);

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CETradeClientApp object

CETradeClientApp theApp;


// CETradeClientApp initialization

BOOL CETradeClientApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	/*SUPER IMPORTANT!!!
	You can init the logging module before "InitializeCef", however, you SHOULD NOT do any logging before
	"InitializeCef". Because "InitializeCef" will spawn multiprocess, if any logging were made before "InitializeCef",
	each process will have it's own log file, thus multiple log files were created.
	Further more, only the main process's log file contains complete logging records, other processes spawned by 
	"InitializeCef" only partially contains the logging records made before "InitializeCef".
	*/

	const uint32_t kMaxLogFileSizeKB = 100; // Each log file stores maximum 100 kb.
	const std::string kLogFileNamePrefix("logfile");
	Logging::Init(Logging::Level::kTrace, kLogFileNamePrefix, kMaxLogFileSizeKB);

	try
	{
		// CEF initialization may crash when closing a browser due to some uncertain CEF bug.
		// This doesn't happen commonly and is very hard to replicate.
		// We are not sure whether it's due to CEF's bug or some incorrect usage of CEF library.
		// So we can only log this fatal error info and analyze later.
		m_cef_initialized = InitializeCef();
	}
	catch (...)
	{
		LOG_FATAL(L"CEF初始化失败！程序关闭。");
		return FALSE;
	}
	
	LOG_TRACE(L"程序启动。"); // Start logging after the "InitializeCef" was called.

	if (!m_cef_initialized)
	{
		LOG_ERROR(L"CEF初始化失败！程序关闭。");
		return FALSE;
	}
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		LOG_ERROR(L"初始化OLE库失败！程序关闭。");
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(APP_ID);

	// First, do login job.
	if (!CLoginDialog(L"欢迎使用地利电子交易结算柜员系统！").Launch())
	{
		LOG_TRACE(L"登录系统未完成，程序关闭。");
		return FALSE;
	}

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* main_frm = new CMainFrame;
	if (!main_frm)
	{
		LOG_ERROR(L"系统内存不足！创建CMainFrame对象失败，程序关闭。");
		return FALSE;
	}
	m_pMainWnd = main_frm;
	// create and load the frame with its resources
	main_frm->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// Then launch the main window.
	if (!main_frm->Launch())
	{
		LOG_ERROR(L"主窗口启动失败，程序关闭。");
		return FALSE;
	}

	// The one and only window has been initialized, so show and update it
	main_frm->ShowWindow(SW_SHOWMAXIMIZED);
	main_frm->UpdateWindow();

	return TRUE;
}

int CETradeClientApp::ExitInstance()
{
	if (m_cef_initialized)
	{
		// closing stop work loop
		m_cef_initialized = false;
		// release CEF app
		m_browser_app = nullptr;
		// shutdown CEF
		CefShutdown();
	}

	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}

//BOOL CETradeClientApp::PumpMessage()
//{
//	if(m_cef_initialized)
//		CefDoMessageLoopWork(); // Integrate with an existing application message loop instead of running its own message loop
//	return CWinApp::PumpMessage();
//}


bool CETradeClientApp::InitializeCef()
{
	void* sandbox_info = NULL;
#if CEF_ENABLE_SANDBOX
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	m_browser_app = new BrowserApp();
	CefMainArgs main_args(GetModuleHandle(NULL)); // get arguments

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, m_browser_app.get(), NULL);
	if (exit_code >= 0)
		return false; // Indicate the initialization is done and should not continue to next step.

	namespace fs = boost::filesystem;
	wchar_t exe_path[MAX_PATH];
	HMODULE h_module = GetModuleHandle(NULL);
	if (NULL != h_module)
		GetModuleFileName(h_module, exe_path, sizeof(exe_path));
	else
		throw std::exception("Calling windows API to get exe path failed.");
	std::string exe_path_ = wstr_2_str(fs::wpath(exe_path).parent_path().wstring());

	// setup settings
	CefSettings settings;
	settings.command_line_args_disabled = true; // Won't accept command line argument.
	settings.multi_threaded_message_loop = true; // Set to true to have the browser process message loop run in a separate thread.
	CefString(&settings.log_file) = exe_path_ + "\\cef\\cef.log";
	CefString(&settings.resources_dir_path) = exe_path_ + "\\cef\\Resources"; // WARNING: 'resources_dir_path' has to be absoulute path.
	CefString(&settings.locales_dir_path) = exe_path_ + "\\cef\\Resources\\locales";
	// 不使用缓存，避免在某些环境下，cef无法使用缓存路径下的数据，造成页面无法显示，注意： 如果缓存路径设置为空，将不支持HTML5 LocalStorage。
	CefString(&settings.cache_path) = "";

	return CefInitialize(main_args, settings, m_browser_app.get(), sandbox_info);
	//@}
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CETradeClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CETradeClientApp message handlers



