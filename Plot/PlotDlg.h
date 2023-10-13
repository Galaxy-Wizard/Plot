
// PlotDlg.h : header file
//

#pragma once

#include <list>

#include "PlotArea.h"

#include "SMP\SimpleMathParser.h" 

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
	std::vector<COLORREF> point_color;
	double original_list_size;

	const double x_shift = 400.0;

	std::vector<COLORREF> colors;

	void InitializeApplication();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCalculate();
	afx_msg void OnBnClickedButtonPlot();
	afx_msg void OnSize(UINT nType, int cx, int cy);

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

	CButton check_1;
	CButton check_2;
	CButton check_3;
	CButton check_4;
	CButton check_5;
	CButton check_6;
	CButton check_7;
	CButton check_8;
	CButton check_9;

	afx_msg void OnBnClickedButtonColors();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonDefaults();

	CEdit edit_1;
	CEdit edit_2;
	CEdit edit_3;
	CEdit edit_4;
	CEdit edit_5;
	CEdit edit_6;
	CEdit edit_7;
	CEdit edit_8;
	CEdit edit_9;

	afx_msg void OnBnClickedButtonAnimate();
	CEdit edit_10;

	void Calculate();
	void Plot();
	CString Save();
	void SaveValues();

	smp::Expression X_Expression;
	smp::Expression Y_Expression;

	double x(double t, double a, double b);
	double y(double t, double a, double b);

	CEdit edit_x_t;
	CEdit edit_y_t;

	bool formula_is_correct;

	CEdit edit_picture_area_size;

	afx_msg void OnBnClickedButtonSaveDigital();
	afx_msg void OnBnClickedButtonLoadDigital();

	CEdit edit_a_u;
	CEdit edit_b_v;

	smp::Expression A_Expression;
	smp::Expression B_Expression;

	double a(double u);
	double b(double v);

	CEdit edit_u_1;
	CEdit edit_u_n;
	CEdit edit_u_step;

	CEdit edit_v_1;
	CEdit edit_v_n;
	CEdit edit_v_step;

	CEdit edit_scale_factor;
	CEdit edit_static_scale;
};
