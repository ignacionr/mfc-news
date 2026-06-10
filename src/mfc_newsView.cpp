#include "framework.h"
#include "mfc_news.h"
#include "mfc_newsDoc.h"
#include "mfc_newsView.h"
#include "MainFrm.h"
#include "ArticleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Dialog for adding a new feed
class CAddFeedDlg : public CDialogEx
{
public:
	CAddFeedDlg(CWnd* pParent = nullptr) : CDialogEx(IDD_ADD_FEED, pParent) {}

	CString m_strTitle;
	CString m_strURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override
	{
		CDialogEx::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_FEED_TITLE, m_strTitle);
		DDX_Text(pDX, IDC_FEED_URL, m_strURL);
	}
};

IMPLEMENT_DYNCREATE(CMfcNewsView, CListView)

BEGIN_MESSAGE_MAP(CMfcNewsView, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CMfcNewsView::OnSelchanged)
	ON_COMMAND(ID_FEED_ADD, &CMfcNewsView::OnFeedAdd)
	ON_COMMAND(ID_FEED_REFRESH, &CMfcNewsView::OnFeedRefresh)
END_MESSAGE_MAP()

CMfcNewsView::CMfcNewsView() noexcept
{
	WriteDebugLog("[DEBUG] CMfcNewsView constructor");
}

CMfcNewsView::~CMfcNewsView()
{
	WriteDebugLog("[DEBUG] CMfcNewsView destructor");
}

BOOL CMfcNewsView::PreCreateWindow(CREATESTRUCT& cs)
{
	WriteDebugLog("[DEBUG] CMfcNewsView::PreCreateWindow start");
	if (!CListView::PreCreateWindow(cs))
	{
		WriteDebugLog("[DEBUG] CMfcNewsView::PreCreateWindow failed");
		return FALSE;
	}

	cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	WriteDebugLog("[DEBUG] CMfcNewsView::PreCreateWindow succeeded");
	return TRUE;
}

void CMfcNewsView::OnInitialUpdate()
{
	WriteDebugLog("[DEBUG] CMfcNewsView::OnInitialUpdate start");
	CListView::OnInitialUpdate();

	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	listCtrl.InsertColumn(0, _T("Title"), LVCFMT_LEFT, 240);
	listCtrl.InsertColumn(1, _T("Date"), LVCFMT_LEFT, 110);

	WriteDebugLog("[DEBUG] CMfcNewsView::OnInitialUpdate calling OnUpdate");
	OnUpdate(NULL, 0, NULL);
	WriteDebugLog("[DEBUG] CMfcNewsView::OnInitialUpdate complete");
}

// Helper to convert UTF-8 string to MFC CString
static CString Utf8ToCString(const std::string& utf8_str)
{
	if (utf8_str.empty()) return _T("");
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), (int)utf8_str.length(), NULL, 0);
	std::wstring wstr(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), (int)utf8_str.length(), &wstr[0], len);
	return CString(wstr.c_str());
}

void CMfcNewsView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	WriteDebugLog("[DEBUG] CMfcNewsView::OnUpdate start");
	CListCtrl& listCtrl = GetListCtrl();
	if (!listCtrl.GetSafeHwnd())
	{
		WriteDebugLog("[DEBUG] CMfcNewsView::OnUpdate - GetSafeHwnd returned NULL");
		return;
	}

	listCtrl.DeleteAllItems();

	CMfcNewsDoc* pDoc = GetDocument();
	if (!pDoc) 
	{
		WriteDebugLog("[DEBUG] CMfcNewsView::OnUpdate - GetDocument returned NULL");
		return;
	}

	const auto& items = pDoc->GetItems();
	for (size_t i = 0; i < items.size(); ++i)
	{
		CString title = Utf8ToCString(items[i].title);
		int index = listCtrl.InsertItem((int)i, title);

		std::time_t t = std::chrono::system_clock::to_time_t(items[i].updated);
		std::tm tm;
		localtime_s(&tm, &t);
		wchar_t buf[64];
		wcsftime(buf, 64, L"%Y-%m-%d %H:%M", &tm);
		listCtrl.SetItemText(index, 1, buf);

		listCtrl.SetItemData(index, i);
	}
	WriteDebugLog("[DEBUG] CMfcNewsView::OnUpdate end");
}

void CMfcNewsView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		int index = pNMLV->iItem;
		DWORD_PTR itemIndex = GetListCtrl().GetItemData(index);

		CMfcNewsDoc* pDoc = GetDocument();
		const auto& items = pDoc->GetItems();
		if (itemIndex < items.size())
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if (pFrame)
			{
				CArticleView* pArticleView = (CArticleView*)pFrame->m_wndSplitter.GetPane(0, 1);
				if (pArticleView)
				{
					pArticleView->SetArticle(items[itemIndex]);
				}
			}
		}
	}
	*pResult = 0;
}

void CMfcNewsView::OnFeedAdd()
{
	CAddFeedDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_strTitle.IsEmpty() || dlg.m_strURL.IsEmpty())
		{
			AfxMessageBox(_T("Title and URL cannot be empty."));
			return;
		}
		CMfcNewsDoc* pDoc = GetDocument();
		if (pDoc)
		{
			pDoc->AddFeed(dlg.m_strTitle, dlg.m_strURL);
		}
	}
}

void CMfcNewsView::OnFeedRefresh()
{
	CMfcNewsDoc* pDoc = GetDocument();
	if (pDoc)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame)
		{
			pFrame->SetMessageText(_T("Refreshing feeds..."));
		}
		
		pDoc->RefreshAllFeeds();
		
		if (pFrame)
		{
			pFrame->SetMessageText(_T("Feeds refreshed successfully."));
		}
	}
}

void CMfcNewsView::OnDraw(CDC* /*pDC*/)
{
}

#ifdef _DEBUG
void CMfcNewsView::AssertValid() const
{
	CListView::AssertValid();
}

void CMfcNewsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMfcNewsDoc* CMfcNewsView::GetDocument() const noexcept
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcNewsDoc)));
	return (CMfcNewsDoc*)m_pDocument;
}
#endif //_DEBUG
