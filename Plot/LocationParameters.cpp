// LocationParameters.cpp : implementation file
//

#include "pch.h"
#include "Plot.h"
#include "afxdialogex.h"
#include "PlotDlg.h"
#include "LocationParameters.h"


// LocationParameters dialog

IMPLEMENT_DYNAMIC(LocationParameters, CDialogEx)

LocationParameters::LocationParameters(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOCATION_PARAMETERS, pParent)
{
	plot_dialog = nullptr;
}

LocationParameters::~LocationParameters()
{
}

void LocationParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LocationParameters, CDialogEx)
END_MESSAGE_MAP()


// LocationParameters message handlers
