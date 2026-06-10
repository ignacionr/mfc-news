#include "framework.h"
#include "mfc_news.h"
#include "MainFrm.h"
#include "mfc_newsView.h"
#include "ArticleView.h"

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
	if (CFrameWnd::OnCreate(lpcs) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	// Create a static splitter with 1 row and 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	// View 0: The list view of articles (left pane, initial width 350)
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CMfcNewsView), CSize(350, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	// View 1: The detail view of the selected article (right pane)
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CArticleView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

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
