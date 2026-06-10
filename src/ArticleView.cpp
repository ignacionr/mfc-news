#include "framework.h"
#include "mfc_news.h"
#include "ArticleView.h"
#include <chrono>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CArticleView, CEditView)

BEGIN_MESSAGE_MAP(CArticleView, CEditView)
END_MESSAGE_MAP()

CArticleView::CArticleView() noexcept
{
	WriteDebugLog("[DEBUG] CArticleView constructor");
}

CArticleView::~CArticleView()
{
	WriteDebugLog("[DEBUG] CArticleView destructor");
}

BOOL CArticleView::PreCreateWindow(CREATESTRUCT& cs)
{
	WriteDebugLog("[DEBUG] CArticleView::PreCreateWindow start");
	if (!CEditView::PreCreateWindow(cs))
	{
		WriteDebugLog("[DEBUG] CArticleView::PreCreateWindow failed");
		return FALSE;
	}

	// Multi-line, read-only edit control with scrolling
	cs.style |= ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL;
	WriteDebugLog("[DEBUG] CArticleView::PreCreateWindow succeeded");
	return TRUE;
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

void CArticleView::SetArticle(const media::rss::feed_item& item)
{
	CString text;

	text += _T("Title: ") + Utf8ToCString(item.title) + _T("\r\n");

	std::time_t t = std::chrono::system_clock::to_time_t(item.updated);
	std::tm tm;
	localtime_s(&tm, &t);
	wchar_t date_buf[64];
	wcsftime(date_buf, 64, L"%Y-%m-%d %H:%M:%S", &tm);
	text += _T("Date: ") + CString(date_buf) + _T("\r\n");

	text += _T("Link: ") + Utf8ToCString(item.link) + _T("\r\n");

	text += _T("Has Playable Media: ") + CString(item.has_media() ? _T("Yes") : _T("No")) + _T("\r\n");
	if (item.has_media())
	{
		text += _T("Best Media URL: ") + Utf8ToCString(item.get_best_media_url()) + _T("\r\n");
	}

	text += _T("Extracted Media Count: ") + CString(std::to_wstring(item.extracted_media_urls.size()).c_str()) + _T("\r\n");
	for (size_t i = 0; i < item.extracted_media_urls.size(); ++i)
	{
		text += _T("  - [") + Utf8ToCString(item.extracted_media_urls[i].type) + _T("/") + 
		        Utf8ToCString(item.extracted_media_urls[i].format) + _T("] ") + 
		        Utf8ToCString(item.extracted_media_urls[i].url) + _T("\r\n");
	}

	text += _T("\r\n----------------------------------------------------------------------\r\n\r\n");

	text += Utf8ToCString(item.description) + _T("\r\n");

	GetEditCtrl().SetWindowText(text);
}

#ifdef _DEBUG
void CArticleView::AssertValid() const
{
	CEditView::AssertValid();
}

void CArticleView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG
