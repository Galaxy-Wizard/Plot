
// Plot.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPlotApp:
// See Plot.cpp for the implementation of this class
//

class CPlotApp : public CWinApp
{
public:
	CPlotApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMultipleobjectsLocation();
};

extern CPlotApp theApp;
