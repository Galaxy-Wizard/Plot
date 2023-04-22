
// PlotDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Plot.h"
#include "PlotDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlotDlg dialog

const auto IDC_STATIC_PLOT_AREA = 1015;

CPlotDlg::CPlotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLOT_DIALOG, pParent), original_list_size(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTING_VALUE, starting_value);
	DDX_Control(pDX, IDC_EDIT_ENDING_VALUE, ending_value);
	DDX_Control(pDX, IDC_EDIT_STEP, step);
	DDX_Control(pDX, IDC_CHECK_X_SYMMETRIC, symmetric_x);
	DDX_Control(pDX, IDC_CHECK_Y_SYMMETRIC, symmetric_y);
	DDX_Control(pDX, IDC_CHECK_XY_SYMMETRIC, symmetric_xy);
	DDX_Control(pDX, IDC_EDIT_ROTATION_ANGLE, rotation_angle);
	DDX_Control(pDX, IDC_CHECK_LINES, lines);
	DDX_Control(pDX, IDC_EDIT_ROTATION_ANGLE_LINES, rotation_angle_lines);
	DDX_Control(pDX, IDC_EDIT_SPRAY_RADIUS, spray_radius);
	DDX_Control(pDX, IDC_EDIT_SPRAY_POINTS_QUANTITY, spray_quantity);
	DDX_Control(pDX, IDC_EDIT_SPRAY_GROWING_FACTOR, spray_growing_factor);
	DDX_Control(pDX, IDC_EDIT_LINES_QUANTITY, lines_quantity);
	DDX_Control(pDX, IDC_CHECK_ALLOW_DRAWING, allow_drawing);
	DDX_Control(pDX, IDC_CHECK_SHOW_AXES, show_axes);
	DDX_Control(pDX, IDC_STATIC_POINTS_QUANTITY, points_quantity);
}

BEGIN_MESSAGE_MAP(CPlotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CPlotDlg::OnBnClickedButtonCalculate)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PLOT, &CPlotDlg::OnBnClickedButtonPlot)
END_MESSAGE_MAP()


// CPlotDlg message handlers

BOOL CPlotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	CRect client_rectangle;

	GetClientRect(&client_rectangle);

	double minimum_square(0.0);

	double width = client_rectangle.Width() - plot_area.x_shift - x_shift;
	double height = client_rectangle.Height();

	if (width < height)
	{
		minimum_square = width;
	}
	else
	{
		minimum_square = height;
	}

	if (
		plot_area.CreateEx(WS_EX_CLIENTEDGE, CString("Static"), CString("Plot area"), WS_CHILD, CRect(int(x_shift), 0, int(minimum_square + plot_area.x_shift + x_shift), int(minimum_square)), this, IDC_STATIC_PLOT_AREA)
		== TRUE
		)
	{
		plot_area.ShowWindow(SW_SHOW);
	}

	starting_value.SetWindowTextW(CString(L"0.0"));
	ending_value.SetWindowTextW(CString(L"10.0"));
	step.SetWindowTextW(CString(L"0.007"));

	symmetric_x.SetCheck(0);
	symmetric_y.SetCheck(0);
	symmetric_xy.SetCheck(0);

	rotation_angle.SetWindowTextW(CString(L"0.5"));

	lines.SetCheck(1);

	lines_quantity.SetWindowTextW(CString(L"2.0"));
	rotation_angle_lines.SetWindowTextW(CString(L"1.0"));

	spray_radius.SetWindowTextW(CString(L"0.007"));
	spray_quantity.SetWindowTextW(CString(L"20.0"));
	spray_growing_factor.SetWindowTextW(CString(L"1619.0"));

	allow_drawing.SetCheck(1);

	CString points_quantity_string;

	points_quantity_string.Format(CString("Points quantity is 2 x %d"), int(list_x.size()));

	points_quantity.SetWindowTextW(points_quantity_string);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

double x(double t)
{
	double result(0.0);

	result = -1.0 * t * sin(3.1415 * t / 4.0);

	return result;
}

double y(double t)
{
	double result(0.0);

	result = 1.0 * t * cos(3.1415 * t / 4.0);

	return result;
}

void CPlotDlg::OnBnClickedButtonCalculate()
{
	Calculate();
}

void CPlotDlg::Calculate()
{
	list_x.clear();
	list_y.clear();

	CString starting_value_string;
	CString ending_value_string;
	CString step_string;

	starting_value.GetWindowTextW(starting_value_string);
	ending_value.GetWindowTextW(ending_value_string);
	step.GetWindowTextW(step_string);

	double starting_t(_wtof(starting_value_string));
	double ending_t(_wtof(ending_value_string));
	double step_t(_wtof(step_string));

	if (step_t == 0.0)
	{
		return;
	}

	for (double t = starting_t; t < ending_t - step_t * 0.5; t += step_t)
	{
		double value_x = x(t);
		double value_y = y(t);

		list_x.push_back(value_x);
		list_y.push_back(value_y);
	}

	if (list_x.size() != 0)
	{
		if (list_x.size() == list_y.size())
		{
			original_list_size = double(list_x.size());

			{
				CString spray_radius_string;

				spray_radius.GetWindowTextW(spray_radius_string);

				double spray_radius_double(_wtof(spray_radius_string));

				CString spray_quantity_string;

				spray_quantity.GetWindowTextW(spray_quantity_string);

				double spray_quantity_double(_wtof(spray_quantity_string));

				CString spray_growing_factor_string;

				spray_growing_factor.GetWindowTextW(spray_growing_factor_string);

				double spray_growing_factor_double(_wtof(spray_growing_factor_string));

				if (spray_radius_double != 0.0 && spray_quantity_double != 0.0 && spray_growing_factor_double != 0.0)
				{
					size_t size = list_x.size();

					std::list<double>::iterator& list_x_i = list_x.begin();
					std::list<double>::iterator& list_y_i = list_y.begin();

					for (int counter = 0; counter<int(size); counter++)
					{
						auto x_s = *list_x_i;
						auto y_s = *list_y_i;

						for (int quantity_counter = 0; quantity_counter<int(spray_quantity_double); quantity_counter++)
						{
							double integer_part = 0.0;
							double fractional_part = std::modf(1.0 * counter / original_list_size, &integer_part);

							auto r_x = x_s + spray_radius_double * 1.0 * double(rand() - RAND_MAX / 2) / RAND_MAX * spray_growing_factor_double * fractional_part;
							auto r_y = y_s + spray_radius_double * 1.0 * double(rand() - RAND_MAX / 2) / RAND_MAX * spray_growing_factor_double * fractional_part;

							list_x.push_back(r_x);
							list_y.push_back(r_y);
						}

						list_x_i++;
						list_y_i++;
					}

					for (int counter = 0; counter<original_list_size; counter++)
					{
						list_x.pop_front();
						list_y.pop_front();
					}
				}
			}

			if (symmetric_x.GetCheck() > 0)
			{
				size_t size = list_x.size();

				std::list<double>::iterator& list_x_i = list_x.begin();
				std::list<double>::iterator& list_y_i = list_y.begin();

				for (int counter = 0; counter<int(size); counter++)
				{
					list_x.push_back(*list_x_i);
					list_y.push_back(-*list_y_i);

					list_x_i++;
					list_y_i++;
				}
			}

			if (symmetric_y.GetCheck() > 0)
			{
				size_t size = list_x.size();

				std::list<double>::iterator& list_x_i = list_x.begin();
				std::list<double>::iterator& list_y_i = list_y.begin();

				for (int counter = 0; counter<int(size); counter++)
				{
					list_x.push_back(-*list_x_i);
					list_y.push_back(*list_y_i);

					list_x_i++;
					list_y_i++;
				}
			}

			if (symmetric_xy.GetCheck() > 0)
			{
				size_t size = list_x.size();

				std::list<double>::iterator& list_x_i = list_x.begin();
				std::list<double>::iterator& list_y_i = list_y.begin();

				for (int counter = 0; counter<int(size); counter++)
				{
					list_x.push_back(-*list_x_i);
					list_y.push_back(-*list_y_i);

					list_x_i++;
					list_y_i++;
				}
			}

			{
				CString rotation_angle_string;

				rotation_angle.GetWindowTextW(rotation_angle_string);

				double rotation_angle_double(3.1415 * _wtof(rotation_angle_string));

				if (rotation_angle_double != 0.0)
				{
					size_t size = list_x.size();

					std::list<double>::iterator& list_x_i = list_x.begin();
					std::list<double>::iterator& list_y_i = list_y.begin();

					for (int counter = 0; counter<int(size); counter++)
					{
						auto x_s = *list_x_i;
						auto y_s = *list_y_i;

						*list_x_i = +1.0 * x_s * cos(rotation_angle_double) + +1.0 * y_s * sin(rotation_angle_double);
						*list_y_i = -1.0 * x_s * sin(rotation_angle_double) + +1.0 * y_s * cos(rotation_angle_double);

						list_x_i++;
						list_y_i++;
					}
				}
			}

			if (lines.GetCheck() > 0)
			{
				CString lines_quantity_string;

				lines_quantity.GetWindowTextW(lines_quantity_string);

				double lines_quantity_double(_wtof(lines_quantity_string));

				CString rotation_angle_lines_string;

				rotation_angle_lines.GetWindowTextW(rotation_angle_lines_string);

				double rotation_angle_lines_double(3.1415 * _wtof(rotation_angle_lines_string));

				if (rotation_angle_lines_double != 0.0)
				{
					size_t size = list_x.size();

					for (size_t lines_quantity_counter = 1; lines_quantity_counter < lines_quantity_double; lines_quantity_counter++)
					{

						std::list<double>::iterator& list_x_i = list_x.begin();
						std::list<double>::iterator& list_y_i = list_y.begin();

						for (size_t counter = 0; counter < size; counter++)
						{
							auto x_s = *list_x_i;
							auto y_s = *list_y_i;

							auto r_x = +1.0 * x_s * cos(lines_quantity_counter * rotation_angle_lines_double) + +1.0 * y_s * sin(lines_quantity_counter * rotation_angle_lines_double);
							auto r_y = -1.0 * x_s * sin(lines_quantity_counter * rotation_angle_lines_double) + +1.0 * y_s * cos(lines_quantity_counter * rotation_angle_lines_double);

							list_x.push_back(r_x);
							list_y.push_back(r_y);

							list_x_i++;
							list_y_i++;
						}
					}
				}
			}
		}
	}

	CString points_quantity_string;

	points_quantity_string.Format(CString("Points quantity is 2 x %d"), int(list_x.size()));

	points_quantity.SetWindowTextW(points_quantity_string);

	plot_area.SetLists(list_x, list_y);
}

void CPlotDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (plot_area.m_hWnd != nullptr)
	{
		if (!plot_area.in_paint)
		{
			if (
				plot_area.DestroyWindow()
				== TRUE
				)
			{

			}
		}
	}
}


void CPlotDlg::OnBnClickedButtonPlot()
{
	if (allow_drawing.GetCheck() > 0)
	{
		if (plot_area.m_hWnd == nullptr)
		{
			CRect client_rectangle;

			GetClientRect(&client_rectangle);

			double minimum_square(0.0);

			double width = client_rectangle.Width() - plot_area.x_shift - x_shift;
			double height = client_rectangle.Height();

			if (width < height)
			{
				minimum_square = width;
			}
			else
			{
				minimum_square = height;
			}

			if (
				plot_area.CreateEx(WS_EX_CLIENTEDGE, CString("Static"), CString("Plot area"), WS_CHILD, CRect(int(x_shift), 0, int(minimum_square + plot_area.x_shift + x_shift), int(minimum_square)), this, IDC_STATIC_PLOT_AREA)
				== TRUE
				)
			{
				plot_area.ShowWindow(SW_SHOW);
			}
		}

		plot_area.show_axes = show_axes.GetCheck();

		plot_area.RedrawWindow();
	}
}

