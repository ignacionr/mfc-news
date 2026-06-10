#pragma once

#include "mfc_newsDoc.h"

class CMfcNewsView : public CListView
{
protected:
	CMfcNewsView() noexcept;
	DECLARE_DYNCREATE(CMfcNewsView)

public:
	CMfcNewsDoc* GetDocument() const noexcept;

public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

public:
	virtual ~CMfcNewsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFeedAdd();
	afx_msg void OnFeedRefresh();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mfc_newsView.cpp
inline CMfcNewsDoc* CMfcNewsView::GetDocument() const noexcept
   { return reinterpret_cast<CMfcNewsDoc*>(m_pDocument); }
#endif
