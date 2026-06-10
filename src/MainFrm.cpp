#include "framework.h"
#include "mfc_news.h"
#include "MainFrm.h"
#include "mfc_newsView.h"
#include "ArticleView.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() noexcept
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpcs)
{
	std::ofstream log("mfcnews_debug.log", std::ios::app);
	log << "[DEBUG] CMainFrame::OnCreate start" << std::endl;

	if (CFrameWnd::OnCreate(lpcs) == -1)
	{
		log << "[DEBUG] CFrameWnd::OnCreate failed" << std::endl;
		return -1;
	}
	log << "[DEBUG] CFrameWnd::OnCreate completed" << std::endl;

	if (!m_wndStatusBar.Create(this))
	{
		log << "[DEBUG] m_wndStatusBar.Create failed" << std::endl;
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	log << "[DEBUG] m_wndStatusBar.Create succeeded" << std::endl;

	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	log << "[DEBUG] CMainFrame::OnCreate completed successfully" << std::endl;

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	std::ofstream log("mfcnews_debug.log", std::ios::app);
	log << "[DEBUG] CMainFrame::OnCreateClient start" << std::endl;

	// Create a static splitter with 1 row and 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		log << "[DEBUG] m_wndSplitter.CreateStatic failed" << std::endl;
		return FALSE;
	}
	log << "[DEBUG] m_wndSplitter.CreateStatic succeeded" << std::endl;

	// View 0: The list view of articles (left pane, initial width 350)
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CMfcNewsView), CSize(350, 100), pContext))
	{
		log << "[DEBUG] m_wndSplitter.CreateView 0 failed" << std::endl;
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	log << "[DEBUG] m_wndSplitter.CreateView 0 succeeded" << std::endl;

	// View 1: The detail view of the selected article (right pane)
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CArticleView), CSize(100, 100), pContext))
	{
		log << "[DEBUG] m_wndSplitter.CreateView 1 failed" << std::endl;
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	log << "[DEBUG] m_wndSplitter.CreateView 1 succeeded" << std::endl;

	log << "[DEBUG] CMainFrame::OnCreateClient completed successfully" << std::endl;
	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPEDWINDOW;
	cs.cx = 900;
	cs.cy = 600;
	cs.lpszName = _T("MfcNews RSS Reader");

	return TRUE;
}

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
