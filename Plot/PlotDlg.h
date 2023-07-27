
// PlotDlg.h : header file
//

#pragma once

#include <list>

#include "PlotArea.h"

// CPlotDlg dialog
class CPlotDlg : public CDialogEx
{
// Construction
public:
	CPlotDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLOT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	std::list<double> list_x;
	std::list<double> list_y;
	double original_list_size;

	const double x_shift = 400.0;

	std::vector<COLORREF> colors;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCalculate();
	afx_msg void OnBnClickedButtonPlot();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void Calculate();

	CEdit starting_value;
	CEdit ending_value;
	CEdit step;
	CButton symmetric_x;
	CButton symmetric_y;
	CButton symmetric_xy;
	CEdit rotation_angle;
	CButton lines;
	CEdit lines_quantity;
	CEdit rotation_angle_lines;
	CEdit spray_radius;
	CEdit spray_quantity;
	CEdit spray_growing_factor;
	PlotArea plot_area;
	CButton allow_drawing;
	CButton show_axes;
	CStatic points_quantity;
	afx_msg void OnBnClickedButtonColors();
	afx_msg void OnBnClickedButtonSave();
};
