#include "framework.h"
#include "mfc_news.h"
#include "mfc_newsDoc.h"
#include "mfc_newsView.h"
#include "MainFrm.h"
#include "ArticleView.h"
#include <map>
#include <atlimage.h>

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

static void DrawDefaultRssIcon(CDC* pDC, const CRect& rect)
{
	// Draw a beautiful orange RSS icon!
	// Background: rounded orange rectangle
	CBrush orangeBrush(RGB(242, 101, 34));
	CBrush* pOldBrush = pDC->SelectObject(&orangeBrush);
	CPen orangePen(PS_SOLID, 1, RGB(242, 101, 34));
	CPen* pOldPen = pDC->SelectObject(&orangePen);
	pDC->RoundRect(rect, CPoint(4, 4));
	
	// Dot at bottom-left: (4, 12) to (7, 15)
	CBrush whiteBrush(RGB(255, 255, 255));
	pDC->SelectObject(&whiteBrush);
	CPen whitePen(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(&whitePen);
	pDC->Ellipse(rect.left + 3, rect.bottom - 6, rect.left + 6, rect.bottom - 3);
	
	// Draw two arcs:
	pDC->SelectStockObject(NULL_BRUSH);
	
	CPen arcPen(PS_SOLID, 2, RGB(255, 255, 255));
	pDC->SelectObject(&arcPen);
	
	// Small arc
	pDC->Arc(rect.left + 1, rect.bottom - 11, rect.left + 9, rect.bottom - 3,
	         rect.left + 1, rect.bottom - 7, rect.left + 5, rect.bottom - 3);
	         
	// Large arc
	pDC->Arc(rect.left - 1, rect.bottom - 15, rect.left + 13, rect.bottom - 1,
	         rect.left - 1, rect.bottom - 9, rect.left + 7, rect.bottom - 1);
	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

static HBITMAP CreateResizedBitmap(CImage& img, int width, int height)
{
	CDC* pDC = CDC::FromHandle(::GetDC(NULL));
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, width, height);
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);
	
	CRect rect(0, 0, width, height);
	memDC.FillSolidRect(&rect, ::GetSysColor(COLOR_WINDOW));
	
	int oldMode = SetStretchBltMode(memDC.GetSafeHdc(), HALFTONE);
	img.Draw(memDC.GetSafeHdc(), rect);
	SetStretchBltMode(memDC.GetSafeHdc(), oldMode);
	
	memDC.SelectObject(pOldBmp);
	::ReleaseDC(NULL, pDC->GetSafeHdc());
	
	return (HBITMAP)bmp.Detach();
}

void CMfcNewsView::OnInitialUpdate()
{
	WriteDebugLog("[DEBUG] CMfcNewsView::OnInitialUpdate start");
	CListView::OnInitialUpdate();

	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	// Setup image list
	m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 4);
	listCtrl.SetImageList(&m_imageList, LVSIL_SMALL);

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

	// Recreate image list to prevent growing/leaks
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 4);
	listCtrl.SetImageList(&m_imageList, LVSIL_SMALL);

	// Add default RSS icon at index 0
	{
		CDC* pScreenDC = CDC::FromHandle(::GetDC(NULL));
		CDC memDC;
		memDC.CreateCompatibleDC(pScreenDC);
		CBitmap bmpDefault;
		bmpDefault.CreateCompatibleBitmap(pScreenDC, 16, 16);
		CBitmap* pOldBmp = memDC.SelectObject(&bmpDefault);
		
		CRect rect(0, 0, 16, 16);
		memDC.FillSolidRect(&rect, ::GetSysColor(COLOR_WINDOW));
		DrawDefaultRssIcon(&memDC, rect);
		
		memDC.SelectObject(pOldBmp);
		::ReleaseDC(NULL, pScreenDC->GetSafeHdc());
		
		m_imageList.Add(&bmpDefault, RGB(255, 255, 255));
	}

	std::map<std::string, int> feedToImageIndex;

	const auto& items = pDoc->GetItems();
	for (size_t i = 0; i < items.size(); ++i)
	{
		const auto& item = items[i];
		int imageIndex = 0; // Default RSS icon

		if (!item.feed_url.empty())
		{
			auto it = feedToImageIndex.find(item.feed_url);
			if (it != feedToImageIndex.end())
			{
				imageIndex = it->second;
			}
			else
			{
				// Try to load the feed icon
				CString feedUrl = Utf8ToCString(item.feed_url);
				CString iconPath = pDoc->GetFeedIconPath(feedUrl);
				if (!iconPath.IsEmpty())
				{
					CImage img;
					if (SUCCEEDED(img.Load(iconPath)))
					{
						HBITMAP hBmp = CreateResizedBitmap(img, 16, 16);
						if (hBmp)
						{
							CBitmap bmp;
							bmp.Attach(hBmp);
							imageIndex = m_imageList.Add(&bmp, RGB(255, 255, 255));
							feedToImageIndex[item.feed_url] = imageIndex;
						}
					}
				}
				
				if (imageIndex == 0)
				{
					feedToImageIndex[item.feed_url] = 0;
				}
			}
		}

		CString title = Utf8ToCString(item.title);
		int index = listCtrl.InsertItem((int)i, title, imageIndex);

		std::time_t t = std::chrono::system_clock::to_time_t(item.updated);
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
