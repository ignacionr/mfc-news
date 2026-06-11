#include "framework.h"
#include "mfc_news.h"
#include "ArticleView.h"
#include <chrono>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CArticleView, CHtmlView)

BEGIN_MESSAGE_MAP(CArticleView, CHtmlView)
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
	if (!CHtmlView::PreCreateWindow(cs))
	{
		WriteDebugLog("[DEBUG] CArticleView::PreCreateWindow failed");
		return FALSE;
	}
	WriteDebugLog("[DEBUG] CArticleView::PreCreateWindow succeeded");
	return TRUE;
}

void CArticleView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
	Navigate(_T("about:blank"));
}

void CArticleView::SetArticle(const media::rss::feed_item& item)
{
	// Get temporary folder path for HTML preview file
	wchar_t temp_path[MAX_PATH];
	if (GetTempPathW(MAX_PATH, temp_path) == 0)
	{
		return;
	}
	std::wstring html_file = std::wstring(temp_path) + L"mfc_news_article.html";

	// Generate a modern, highly styled HTML document
	std::string html;
	html += "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n";
	html += "<style>\n";
	html += "  body {\n";
	html += "    font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Helvetica, Arial, sans-serif;\n";
	html += "    color: #2c3e50;\n";
	html += "    background-color: #f8f9fa;\n";
	html += "    line-height: 1.6;\n";
	html += "    margin: 0;\n";
	html += "    padding: 20px;\n";
	html += "  }\n";
	html += "  .article-card {\n";
	html += "    background: #ffffff;\n";
	html += "    padding: 30px;\n";
	html += "    border-radius: 12px;\n";
	html += "    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);\n";
	html += "    max-width: 750px;\n";
	html += "    margin: 0 auto;\n";
	html += "    border: 1px solid #e9ecef;\n";
	html += "  }\n";
	html += "  h1 {\n";
	html += "    font-size: 26px;\n";
	html += "    color: #1a1a1a;\n";
	html += "    margin-top: 0;\n";
	html += "    margin-bottom: 15px;\n";
	html += "    font-weight: 700;\n";
	html += "    line-height: 1.3;\n";
	html += "  }\n";
	html += "  .meta {\n";
	html += "    font-size: 13px;\n";
	html += "    color: #6c757d;\n";
	html += "    margin-bottom: 25px;\n";
	html += "    border-bottom: 1px solid #dee2e6;\n";
	html += "    padding-bottom: 15px;\n";
	html += "  }\n";
	html += "  .meta a {\n";
	html += "    color: #007bff;\n";
	html += "    text-decoration: none;\n";
	html += "    font-weight: 500;\n";
	html += "  }\n";
	html += "  .meta a:hover {\n";
	html += "    text-decoration: underline;\n";
	html += "  }\n";
	html += "  .image-container {\n";
	html += "    margin-bottom: 25px;\n";
	html += "    text-align: center;\n";
	html += "  }\n";
	html += "  .article-image {\n";
	html += "    max-width: 100%;\n";
	html += "    height: auto;\n";
	html += "    border-radius: 8px;\n";
	html += "    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.08);\n";
	html += "  }\n";
	html += "  .content {\n";
	html += "    font-size: 16px;\n";
	html += "    color: #333333;\n";
	html += "  }\n";
	html += "  .media-badge {\n";
	html += "    display: inline-block;\n";
	html += "    background: #e7f5ff;\n";
	html += "    color: #007bff;\n";
	html += "    padding: 6px 12px;\n";
	html += "    border-radius: 20px;\n";
	html += "    font-size: 12px;\n";
	html += "    font-weight: 600;\n";
	html += "    margin-bottom: 15px;\n";
	html += "  }\n";
	html += "  .feed-source {\n";
	html += "    margin-bottom: 15px;\n";
	html += "    line-height: 20px;\n";
	html += "  }\n";
	html += "  .feed-icon-img, .feed-icon-svg {\n";
	html += "    width: 20px;\n";
	html += "    height: 20px;\n";
	html += "    border-radius: 4px;\n";
	html += "    vertical-align: middle;\n";
	html += "    margin-right: 8px;\n";
	html += "    display: inline-block;\n";
	html += "  }\n";
	html += "  .feed-name {\n";
	html += "    font-size: 14px;\n";
	html += "    font-weight: 600;\n";
	html += "    color: #495057;\n";
	html += "    vertical-align: middle;\n";
	html += "    display: inline-block;\n";
	html += "  }\n";
	html += "</style>\n</head>\n<body>\n";
	html += "<div class=\"article-card\">\n";
	if (!item.feed_title.empty())
	{
		html += "  <div class=\"feed-source\">\n";
		if (!item.feed_icon_url.empty())
		{
			html += "    <img class=\"feed-icon-img\" src=\"" + item.feed_icon_url + "\" onerror=\"this.outerHTML='<svg class=\\'feed-icon-svg\\' viewBox=\\'0 0 24 24\\'><rect width=\\'24\\' height=\\'24\\' rx=\\'6\\' fill=\\'#f26522\\'/><circle cx=\\'6\\' cy=\\'18\\' r=\\'2\\' fill=\\'#fff\\'/><path d=\\'M 4 4 A 20 20 0 0 1 24 24 h -3 A 17 17 0 0 0 4 7 Z\\' fill=\\'#fff\\'/><path d=\\'M 4 11 A 13 13 0 0 1 17 24 h -3 A 10 10 0 0 0 4 14 Z\\' fill=\\'#fff\\'/></svg>';\" />\n";
		}
		else
		{
			html += "    <svg class=\"feed-icon-svg\" viewBox=\"0 0 24 24\"><rect width=\"24\" height=\"24\" rx=\"6\" fill=\"#f26522\"/><circle cx=\"6\" cy=\"18\" r=\"2\" fill=\"#fff\"/><path d=\"M 4 4 A 20 20 0 0 1 24 24 h -3 A 17 17 0 0 0 4 7 Z\" fill=\"#fff\"/><path d=\"M 4 11 A 13 13 0 0 1 17 24 h -3 A 10 10 0 0 0 4 14 Z\" fill=\"#fff\"/></svg>\n";
		}
		html += "    <span class=\"feed-name\">" + item.feed_title + "</span>\n";
		html += "  </div>\n";
	}

	if (item.has_media())
	{
		html += "  <div class=\"media-badge\">🎬 Playable Media Available</div>\n";
	}

	html += "  <h1>" + item.title + "</h1>\n";

	std::time_t t = std::chrono::system_clock::to_time_t(item.updated);
	std::tm tm_info;
	localtime_s(&tm_info, &t);
	wchar_t date_buf[64];
	wcsftime(date_buf, 64, L"%Y-%m-%d %H:%M:%S", &tm_info);
	std::wstring wdate(date_buf);
	std::string date_str(wdate.begin(), wdate.end());

	html += "  <div class=\"meta\">\n";
	html += "    Published: " + date_str + " | <a href=\"" + item.link + "\" target=\"_blank\">Open in Browser</a>\n";
	html += "  </div>\n";

	if (!item.image_url.empty())
	{
		html += "  <div class=\"image-container\">\n";
		html += "    <img class=\"article-image\" src=\"" + item.image_url + "\" />\n";
		html += "  </div>\n";
	}

	html += "  <div class=\"content\">\n";
	html += "    " + item.description + "\n";
	html += "  </div>\n";
	html += "</div>\n</body>\n</html>\n";

	// Write UTF-8 HTML file
	FILE* f = nullptr;
	_wfopen_s(&f, html_file.c_str(), L"wb");
	if (f)
	{
		fwrite(html.c_str(), 1, html.length(), f);
		fclose(f);
	}

	// Navigate to preview file
	Navigate(html_file.c_str());
}

#ifdef _DEBUG
void CArticleView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CArticleView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG
