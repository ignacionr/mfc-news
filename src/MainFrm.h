#pragma once

class CMainFrame : public CFrameWnd
{
protected:
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CSplitterWnd      m_wndSplitter;

protected:
	CStatusBar        m_wndStatusBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	DECLARE_MESSAGE_MAP()
};
