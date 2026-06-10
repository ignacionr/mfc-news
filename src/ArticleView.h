#pragma once

#include "rss/feed_item.hpp"

class CArticleView : public CEditView
{
protected:
	CArticleView() noexcept;
	DECLARE_DYNCREATE(CArticleView)

public:
	void SetArticle(const media::rss::feed_item& item);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~CArticleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
