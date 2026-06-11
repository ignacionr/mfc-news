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
	WriteDebugLog("[DEBUG] CMfcNewsDoc constructor");
}

CMfcNewsDoc::~CMfcNewsDoc()
{
	WriteDebugLog("[DEBUG] CMfcNewsDoc destructor");
}

BOOL CMfcNewsDoc::OnNewDocument()
{
	WriteDebugLog("[DEBUG] OnNewDocument start");

	if (!CDocument::OnNewDocument())
	{
		WriteDebugLog("[DEBUG] CDocument::OnNewDocument failed");
		return FALSE;
	}

	// Populate with some premium default RSS feeds so the user is wowed immediately
	m_feeds.clear();
	m_feeds.emplace_back(_T("BBC News"), _T("http://feeds.bbci.co.uk/news/rss.xml"));
	m_feeds.emplace_back(_T("NASA Breaking News"), _T("https://www.nasa.gov/news-release/feed/"));
	m_feeds.emplace_back(_T("Los Temas del Día"), _T("https://www.spreaker.com/show/4209606/episodes/feed"));
	
	WriteDebugLog("[DEBUG] RefreshAllFeeds start");
	// Refresh automatically on startup
	RefreshAllFeeds();
	WriteDebugLog("[DEBUG] RefreshAllFeeds end");

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
	try
	{
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
				pFile->Close();
				delete pFile;
				pFile = nullptr;
			}
		}
		catch (CException* pEx)
		{
			pEx->Delete();
			if (pFile)
			{
				try { pFile->Close(); } catch(...) {}
				delete pFile;
			}
		}
		catch (...)
		{
			if (pFile)
			{
				delete pFile;
			}
		}
		session.Close();
	}
	catch (CException* pEx)
	{
		pEx->Delete();
	}
	catch (...)
	{
	}
	return utf8_content;
}

static CString GetSafeFilename(const CString& url)
{
	CString safe = url;
	safe.Replace(_T("http://"), _T(""));
	safe.Replace(_T("https://"), _T(""));
	safe.Replace(_T("/"), _T("_"));
	safe.Replace(_T(":"), _T("_"));
	safe.Replace(_T("?"), _T("_"));
	safe.Replace(_T("&"), _T("_"));
	safe.Replace(_T("="), _T("_"));
	safe.Replace(_T("."), _T("_"));
	return safe;
}

static CString Utf8ToCString(const std::string& utf8_str)
{
	if (utf8_str.empty()) return _T("");
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), (int)utf8_str.length(), NULL, 0);
	std::wstring wstr(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), (int)utf8_str.length(), &wstr[0], len);
	return CString(wstr.c_str());
}

std::vector<BYTE> CMfcNewsDoc::DownloadBinaryUrl(const CString& url)
{
	std::vector<BYTE> content;
	try
	{
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
					content.insert(content.end(), (BYTE*)buffer, (BYTE*)buffer + bytesRead);
				}
				pFile->Close();
				delete pFile;
				pFile = nullptr;
			}
		}
		catch (CException* pEx)
		{
			pEx->Delete();
			if (pFile)
			{
				try { pFile->Close(); } catch(...) {}
				delete pFile;
			}
		}
		catch (...)
		{
			if (pFile)
			{
				delete pFile;
			}
		}
		session.Close();
	}
	catch (CException* pEx)
	{
		pEx->Delete();
	}
	catch (...)
	{
	}
	return content;
}

CString CMfcNewsDoc::GetFeedIconPath(const CString& feedUrl)
{
	auto it = m_feed_icons.find(feedUrl);
	if (it != m_feed_icons.end())
	{
		return it->second;
	}
	return _T("");
}

void CMfcNewsDoc::RefreshAllFeeds()
{
	try
	{
		m_items.clear();
		
		for (const auto& feed_info : m_feeds)
		{
			try
			{
				std::string xml = DownloadFeedUrl(feed_info.second);
				if (!xml.empty())
				{
					media::rss::feed parser;
					parser(xml);

					// Convert custom feed title to UTF-8
					std::string custom_title;
					{
						int len = WideCharToMultiByte(CP_UTF8, 0, feed_info.first, -1, NULL, 0, NULL, NULL);
						if (len > 0)
						{
							std::string temp(len, 0);
							WideCharToMultiByte(CP_UTF8, 0, feed_info.first, -1, &temp[0], len, NULL, NULL);
							if (temp.back() == '\0') temp.pop_back(); // Remove null terminator if included
							custom_title = temp;
						}
					}

					// Convert feed URL to UTF-8
					std::string feed_url_utf8;
					{
						int len = WideCharToMultiByte(CP_UTF8, 0, feed_info.second, -1, NULL, 0, NULL, NULL);
						if (len > 0)
						{
							std::string temp(len, 0);
							WideCharToMultiByte(CP_UTF8, 0, feed_info.second, -1, &temp[0], len, NULL, NULL);
							if (temp.back() == '\0') temp.pop_back();
							feed_url_utf8 = temp;
						}
					}

					// Check if feed has an icon and download it if needed
					std::string iconUrl = parser.image_url();
					if (!iconUrl.empty())
					{
						CString feedUrl = feed_info.second;
						if (m_feed_icons.find(feedUrl) == m_feed_icons.end())
						{
							// Download the icon
							CString wIconUrl = Utf8ToCString(iconUrl);
							std::vector<BYTE> bytes = DownloadBinaryUrl(wIconUrl);
							if (!bytes.empty())
							{
								// Save to a temp file
								wchar_t temp_path[MAX_PATH];
								if (GetTempPathW(MAX_PATH, temp_path) != 0)
								{
									// Determine extension
									CString ext = _T(".png");
									if (iconUrl.find(".ico") != std::string::npos) ext = _T(".ico");
									else if (iconUrl.find(".jpg") != std::string::npos || iconUrl.find(".jpeg") != std::string::npos) ext = _T(".jpg");
									else if (iconUrl.find(".gif") != std::string::npos) ext = _T(".gif");
									
									CString localPath = CString(temp_path) + GetSafeFilename(feedUrl) + ext;
									FILE* f = nullptr;
									_wfopen_s(&f, localPath, L"wb");
									if (f)
									{
										fwrite(bytes.data(), 1, bytes.size(), f);
										fclose(f);
										m_feed_icons[feedUrl] = localPath;
									}
								}
							}
						}
					}

					for (auto& item : parser.items)
					{
						if (item.feed_title.empty())
						{
							item.feed_title = !parser.feed_title.empty() ? parser.feed_title : custom_title;
						}
						if (item.feed_icon_url.empty())
						{
							item.feed_icon_url = parser.image_url();
						}
						item.feed_url = feed_url_utf8;
					}
					m_items.insert(m_items.end(), parser.items.begin(), parser.items.end());
				}
			}
			catch (const std::exception&)
			{
				// Ignore parsing errors for this feed
			}
			catch (...)
			{
				// Ignore other exceptions
			}
		}

		// Sort articles: newest first
		try
		{
			std::sort(m_items.begin(), m_items.end(), [](const media::rss::feed_item& a, const media::rss::feed_item& b) {
				return a.updated > b.updated;
			});
		}
		catch (...)
		{
		}

		try
		{
			UpdateAllViews(NULL);
		}
		catch (...)
		{
		}
	}
	catch (...)
	{
		// Prevent any other exception from cascading
	}
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
