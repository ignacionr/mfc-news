#pragma once

#include <map>
#include <vector>
#include <utility>
#include <string>
#include "rss/feed_item.hpp"

class CMfcNewsDoc : public CDocument
{
protected:
	CMfcNewsDoc() noexcept;
	DECLARE_DYNCREATE(CMfcNewsDoc)

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Feed operations
	void AddFeed(const CString& title, const CString& url);
	void RefreshAllFeeds();
	
	const std::vector<std::pair<CString, CString>>& GetFeeds() const { return m_feeds; }
	const std::vector<media::rss::feed_item>& GetItems() const { return m_items; }
	CString GetFeedIconPath(const CString& feedUrl);

public:
	virtual ~CMfcNewsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::vector<std::pair<CString, CString>> m_feeds; // Pair of <Title, URL>
	std::vector<media::rss::feed_item> m_items;
	std::map<CString, CString> m_feed_icons; // Map of <Feed URL, Local Icon File Path>

	std::string DownloadFeedUrl(const CString& url);
	std::vector<BYTE> DownloadBinaryUrl(const CString& url);

	DECLARE_MESSAGE_MAP()
};
