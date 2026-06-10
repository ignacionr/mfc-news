#pragma once

#include <afxhtml.h>
#include "rss/feed_item.hpp"

class CArticleView : public CHtmlView
{
protected:
	CArticleView() noexcept;
	DECLARE_DYNCREATE(CArticleView)

public:
	void SetArticle(const media::rss::feed_item& item);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();

public:
	virtual ~CArticleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
