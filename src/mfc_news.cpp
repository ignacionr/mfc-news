#include "framework.h"
#include "mfc_news.h"
#include "MainFrm.h"
#include "mfc_newsDoc.h"
#include "mfc_newsView.h"

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
	WriteDebugLog("[DEBUG] CMfcNewsApp constructor start");
	SetAppID(_T("MfcNews.AppID.NoVersion"));
	WriteDebugLog("[DEBUG] CMfcNewsApp constructor end");
}

// The one and only CMfcNewsApp object
CMfcNewsApp theApp;

// CMfcNewsApp initialization
BOOL CMfcNewsApp::InitInstance()
{
	WriteDebugLog("[DEBUG] InitInstance start");

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	WriteDebugLog("[DEBUG] CWinApp::InitInstance completed");

	if (!AfxOleInit())
	{
		WriteDebugLog("[DEBUG] AfxOleInit failed");
		AfxMessageBox(_T("OLE initialization failed!"));
		return FALSE;
	}
	WriteDebugLog("[DEBUG] AfxOleInit succeeded");

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

	WriteDebugLog("[DEBUG] ProcessShellCommand start");
	if (!ProcessShellCommand(cmdInfo))
	{
		WriteDebugLog("[DEBUG] ProcessShellCommand failed");
		return FALSE;
	}
	WriteDebugLog("[DEBUG] ProcessShellCommand succeeded");

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	WriteDebugLog("[DEBUG] InitInstance end (success)");
	return TRUE;
}

int CMfcNewsApp::ExitInstance()
{
	WriteDebugLog("[DEBUG] ExitInstance start");
	AfxOleTerm(FALSE);
	int ret = CWinApp::ExitInstance();
	WriteDebugLog("[DEBUG] ExitInstance end");
	return ret;
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
