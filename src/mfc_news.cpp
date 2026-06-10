#include "framework.h"
#include "mfc_news.h"
#include "MainFrm.h"
#include "mfc_newsDoc.h"
#include "mfc_newsView.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMfcNewsApp
BEGIN_MESSAGE_MAP(CMfcNewsApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMfcNewsApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// CMfcNewsApp construction
CMfcNewsApp::CMfcNewsApp() noexcept
{
	std::ofstream log("mfcnews_debug.log", std::ios::app);
	log << "[DEBUG] CMfcNewsApp constructor start" << std::endl;
	SetAppID(_T("MfcNews.AppID.NoVersion"));
	log << "[DEBUG] CMfcNewsApp constructor end" << std::endl;
}

// The one and only CMfcNewsApp object
CMfcNewsApp theApp;

// CMfcNewsApp initialization
BOOL CMfcNewsApp::InitInstance()
{
	std::ofstream log("mfcnews_debug.log", std::ios::app);
	log << "[DEBUG] InitInstance start" << std::endl;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	log << "[DEBUG] CWinApp::InitInstance completed" << std::endl;

	if (!AfxOleInit())
	{
		log << "[DEBUG] AfxOleInit failed" << std::endl;
		AfxMessageBox(_T("OLE initialization failed!"));
		return FALSE;
	}
	log << "[DEBUG] AfxOleInit succeeded" << std::endl;

	AfxEnableControlContainer();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMfcNewsDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMfcNewsView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	log << "[DEBUG] ProcessShellCommand start" << std::endl;
	if (!ProcessShellCommand(cmdInfo))
	{
		log << "[DEBUG] ProcessShellCommand failed" << std::endl;
		return FALSE;
	}
	log << "[DEBUG] ProcessShellCommand succeeded" << std::endl;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	log << "[DEBUG] InitInstance end (success)" << std::endl;
	return TRUE;
}

int CMfcNewsApp::ExitInstance()
{
	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX) {}

protected:
	virtual void DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CMfcNewsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
