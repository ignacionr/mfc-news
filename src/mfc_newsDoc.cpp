#include "framework.h"
#include "mfc_news.h"
#include "mfc_newsDoc.h"
#include "rss/feed.hpp"
#include <propkey.h>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMfcNewsDoc, CDocument)

BEGIN_MESSAGE_MAP(CMfcNewsDoc, CDocument)
END_MESSAGE_MAP()

CMfcNewsDoc::CMfcNewsDoc() noexcept
{
}

CMfcNewsDoc::~CMfcNewsDoc()
{
}

BOOL CMfcNewsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Populate with some premium default RSS feeds so the user is wowed immediately
	m_feeds.clear();
	m_feeds.emplace_back(_T("BBC News"), _T("http://feeds.bbci.co.uk/news/rss.xml"));
	m_feeds.emplace_back(_T("NASA Breaking News"), _T("https://www.nasa.gov/news-release/feed/"));
	
	// Refresh automatically on startup
	RefreshAllFeeds();

	return TRUE;
}

void CMfcNewsDoc::AddFeed(const CString& title, const CString& url)
{
	m_feeds.emplace_back(title, url);
	SetModifiedFlag(TRUE);
	RefreshAllFeeds();
}

std::string CMfcNewsDoc::DownloadFeedUrl(const CString& url)
{
	std::string utf8_content;
	CInternetSession session(_T("MfcNewsReader"));
	CHttpFile* pFile = nullptr;
	try
	{
		pFile = (CHttpFile*)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);
		if (pFile)
		{
			char buffer[4096];
			UINT bytesRead = 0;
			while ((bytesRead = pFile->Read(buffer, sizeof(buffer))) > 0)
			{
				utf8_content.append(buffer, bytesRead);
			}
		}
	}
	catch (CInternetException* pEx)
	{
		// Silently catch network errors for offline durability
		pEx->Delete();
	}
	
	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
	session.Close();
	return utf8_content;
}

void CMfcNewsDoc::RefreshAllFeeds()
{
	m_items.clear();
	
	for (const auto& feed_info : m_feeds)
	{
		std::string xml = DownloadFeedUrl(feed_info.second);
		if (!xml.empty())
		{
			media::rss::feed parser;
			parser(xml);
			m_items.insert(m_items.end(), parser.items.begin(), parser.items.end());
		}
	}

	// Sort articles: newest first
	std::sort(m_items.begin(), m_items.end(), [](const media::rss::feed_item& a, const media::rss::feed_item& b) {
		return a.updated > b.updated;
	});

	UpdateAllViews(NULL);
}

void CMfcNewsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (INT_PTR)m_feeds.size();
		for (const auto& feed : m_feeds)
		{
			ar << feed.first << feed.second;
		}
	}
	else
	{
		m_feeds.clear();
		INT_PTR count;
		ar >> count;
		for (INT_PTR i = 0; i < count; ++i)
		{
			CString title, url;
			ar >> title >> url;
			m_feeds.emplace_back(title, url);
		}
		RefreshAllFeeds();
	}
}

#ifdef _DEBUG
void CMfcNewsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfcNewsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
