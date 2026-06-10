#pragma once

#ifndef __AFXWIN_H__
	#error "include 'framework.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

class CMfcNewsApp : public CWinApp
{
public:
	CMfcNewsApp() noexcept;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
};

extern CMfcNewsApp theApp;
