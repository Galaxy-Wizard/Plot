#pragma once
#include "afxdialogex.h"

class CPlotDlg;

// LocationParameters dialog

class LocationParameters : public CDialogEx
{
	DECLARE_DYNAMIC(LocationParameters)

public:
	LocationParameters(CWnd* pParent = nullptr);   // standard constructor
	virtual ~LocationParameters();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOCATION_PARAMETERS };
#endif

	CPlotDlg* plot_dialog;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
