
// PlotDlg.cpp : implementation file
//


#include "pch.h"
#include "framework.h"
#include "Plot.h"
#include "PlotDlg.h"
#include "afxdialogex.h"

#include <list>

#pragma comment(lib, "gdiplus.lib")

#include <GdiPlus.h>

#include "SMP\SimpleMathParser.h" 
#include "SMP\SMPExceptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
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
	DDX_Control(pDX, IDC_CHECK1, check_1);
	DDX_Control(pDX, IDC_CHECK2, check_2);
	DDX_Control(pDX, IDC_CHECK3, check_3);
	DDX_Control(pDX, IDC_CHECK4, check_4);
	DDX_Control(pDX, IDC_CHECK5, check_5);
	DDX_Control(pDX, IDC_CHECK6, check_6);
	DDX_Control(pDX, IDC_CHECK7, check_7);
	DDX_Control(pDX, IDC_CHECK8, check_8);
	DDX_Control(pDX, IDC_CHECK9, check_9);
	DDX_Control(pDX, IDC_EDIT1, edit_1);
	DDX_Control(pDX, IDC_EDIT2, edit_2);
	DDX_Control(pDX, IDC_EDIT3, edit_3);
	DDX_Control(pDX, IDC_EDIT4, edit_4);
	DDX_Control(pDX, IDC_EDIT5, edit_5);
	DDX_Control(pDX, IDC_EDIT6, edit_6);
	DDX_Control(pDX, IDC_EDIT7, edit_7);
	DDX_Control(pDX, IDC_EDIT8, edit_8);
	DDX_Control(pDX, IDC_EDIT9, edit_9);
	DDX_Control(pDX, IDC_EDIT_ANIMATION_STEPS_QUANTITY, edit_10);
	DDX_Control(pDX, IDC_EDIT_X_T, edit_x_t);
	DDX_Control(pDX, IDC_EDIT_Y_T, edit_y_t);
}

BEGIN_MESSAGE_MAP(CPlotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CPlotDlg::OnBnClickedButtonCalculate)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PLOT, &CPlotDlg::OnBnClickedButtonPlot)
	ON_BN_CLICKED(IDC_BUTTON_COLORS, &CPlotDlg::OnBnClickedButtonColors)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPlotDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, &CPlotDlg::OnBnClickedButtonDefaults)
	ON_BN_CLICKED(IDC_BUTTON_ANIMATE, &CPlotDlg::OnBnClickedButtonAnimate)
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

	InitializeApplication();

	X_Expression.setNewXAlias('t');
	Y_Expression.setNewXAlias('t');


	edit_x_t.SetWindowTextW(L" - 1.0 * t * sin(3.1415 * t / 4.0)");
	edit_y_t.SetWindowTextW(L" 1.0 * t * cos(3.1415 * t / 4.0)");

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

double CPlotDlg::x(double t)
{
	double result(0.0);

	X_Expression.setXValue(t);

	result = X_Expression.getResult();

	return result;
}

double CPlotDlg::y(double t)
{
	double result(0.0);

	Y_Expression.setXValue(t);

	result = Y_Expression.getResult();

	return result;
}

void CPlotDlg::OnBnClickedButtonCalculate()
{
	Calculate();
}

void CPlotDlg::Calculate()
{
	CString x_t;
	CString y_t;

	edit_x_t.GetWindowTextW(x_t);
	edit_y_t.GetWindowTextW(y_t);

	try
	{
		X_Expression.setExpression(CStringA(x_t).GetBuffer());
		Y_Expression.setExpression(CStringA(y_t).GetBuffer());
	}
	catch (...)
	{
		AfxMessageBox(L"Incorrect expression in formula(s).", MB_ICONEXCLAMATION);

		return;
	}

	srand((unsigned int)GetTickCount64());

	list_x.clear();
	list_y.clear();

	point_color.clear();

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

		auto weight_number = (ending_t - starting_t) / step_t;
		auto weight = (t / step_t) / weight_number;

		point_color.push_back
		(
			RGB(
				BYTE(((GetRValue(colors[0]) * weight + GetRValue(colors[1]) * (1 - weight)))),
				BYTE(((GetGValue(colors[0]) * weight + GetGValue(colors[1]) * (1 - weight)))),
				BYTE(((GetBValue(colors[0]) * weight + GetBValue(colors[1]) * (1 - weight))))
			)
		);
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

							point_color.push_back(point_color.at(counter));
						}

						list_x_i++;
						list_y_i++;
					}

					for (int counter = 0; counter < original_list_size; counter++)
					{
						list_x.pop_front();
						list_y.pop_front();

						point_color.erase(point_color.begin());
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

					point_color.push_back(point_color.at(counter));
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

					point_color.push_back(point_color.at(counter));
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

					point_color.push_back(point_color.at(counter));
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

							point_color.push_back(point_color.at(counter));
						}
					}
				}
			}
		}
	}

	CString points_quantity_string;

	points_quantity_string.Format(CString("Points quantity is 2 x %d"), int(list_x.size()));

	points_quantity.SetWindowTextW(points_quantity_string);

	plot_area.SetLists(list_x, list_y, point_color);

	plot_area.SetColors(colors);
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
	Plot();
}

void CPlotDlg::Plot()
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



void CPlotDlg::OnBnClickedButtonColors()
{
	CColorDialog color_dialog;
	auto result = color_dialog.DoModal();

	if (result == IDOK)
	{
		auto colors_vector = color_dialog.GetSavedCustomColors();
		for (size_t counter = 0; counter < 16; counter++)
		{
			colors[counter] = colors_vector[counter];
		}
	}
}


void CPlotDlg::OnBnClickedButtonSave()
{
	Save();
}

CString  CPlotDlg::Save()
{
	return plot_area.DrawPicture();
}


void CPlotDlg::OnBnClickedButtonDefaults()
{
	InitializeApplication();
}

void CPlotDlg::InitializeApplication()
{
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

	colors.resize(16);

	colors[0] = RGB(255, 255, 0);
	colors[1] = RGB(0, 255, 255);
	colors[2] = RGB(0, 0, 0);
	colors[3] = RGB(0, 0, 0);

	colors[4] = RGB(0, 0, 0);
	colors[5] = RGB(0, 0, 0);
	colors[6] = RGB(0, 0, 0);
	colors[7] = RGB(0, 0, 0);

	colors[8] = RGB(255, 0, 0);
	colors[9] = RGB(0, 0, 255);
	colors[10] = RGB(255, 255, 255);
	colors[11] = RGB(0, 0, 0);

	colors[12] = RGB(0, 0, 0);
	colors[13] = RGB(0, 0, 0);
	colors[14] = RGB(0, 0, 0);
	colors[15] = RGB(0, 0, 0);

	plot_area.SetColors(colors);

	check_1.SetCheck(0);
	check_2.SetCheck(0);
	check_3.SetCheck(0);
	check_4.SetCheck(1);
	check_5.SetCheck(0);
	check_6.SetCheck(0);
	check_7.SetCheck(0);
	check_8.SetCheck(0);
	check_9.SetCheck(0);

	edit_1.SetWindowTextW(CString(L"0.0"));
	edit_2.SetWindowTextW(CString(L"0.0"));
	edit_3.SetWindowTextW(CString(L"0.0"));
	edit_4.SetWindowTextW(CString(L"0.25"));
	edit_5.SetWindowTextW(CString(L"0.0"));
	edit_6.SetWindowTextW(CString(L"0.0"));
	edit_7.SetWindowTextW(CString(L"0.0"));
	edit_8.SetWindowTextW(CString(L"0.0"));
	edit_9.SetWindowTextW(CString(L"0.0"));
	edit_10.SetWindowTextW(CString(L"10"));
}

void CPlotDlg::OnBnClickedButtonAnimate()
{

	std::list<Gdiplus::Image*> images;

	// Initialize GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status_start = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (status_start == Gdiplus::Ok)
	{

	}
	else
	{
		//AfxMessageBox(L"GdiplusStartup Failed.");
	}

	Gdiplus::Image *resulting_gif = nullptr;
	CStringW file_name;

	Gdiplus::EncoderParameters encoderParameters;
	ULONG parameterValue;
	GUID dimension = Gdiplus::FrameDimensionPage;
	Gdiplus::Status stat;

	// An EncoderParameters object has an array of
	// EncoderParameter objects. In this case, there is only
	// one EncoderParameter object in the array.
	encoderParameters.Count = 1;

	// Initialize the one EncoderParameter object.
	encoderParameters.Parameter[0].Guid = Gdiplus::EncoderSaveFlag;
	encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &parameterValue;

	// Get the CLSID of the GIF encoder.
	CLSID encoderClsid;
	INT  result  = GetEncoderClsid(L"image/gif", &encoderClsid);
	if (result < 0)
	{
		//AfxMessageBox(L"The GIF encoder is not installed.");
	}
	else
	{
		//WCHAR  strGuid[39];

		//StringFromGUID2(encoderClsid, strGuid, 39);
		
		//AfxMessageBox(strGuid);
	}


	auto date_time = COleDateTime::GetTickCount();

	file_name.Format(CString(L"\\Plot%4d.%2d.%2d_%2d-%2d-%2d.gif"),
		date_time.GetYear(), date_time.GetMonth(), date_time.GetDay(),
		date_time.GetHour(), date_time.GetMinute(), date_time.GetSecond());

	file_name = std::filesystem::current_path().c_str() + file_name;

	CString animation_steps_string;

	edit_10.GetWindowTextW(animation_steps_string);

	int animation_steps = _wtoi(animation_steps_string.GetBuffer());


	for (int counter = 0; counter < animation_steps; counter++)
	{
		CString tempramental_string_1;
		CString tempramental_string_2;

		double tempramental_double_1;
		double tempramental_double_2;

		if (check_1.GetCheck() > 0)
		{
			starting_value.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_1.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			starting_value.SetWindowTextW(tempramental_string_2);
		}
		if (check_2.GetCheck() > 0)
		{
			ending_value.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_2.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			ending_value.SetWindowTextW(tempramental_string_2);
		}
		if (check_3.GetCheck() > 0)
		{
			step.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_3.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			step.SetWindowTextW(tempramental_string_2);
		}
		if (check_4.GetCheck() > 0)
		{
			rotation_angle.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_4.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			rotation_angle.SetWindowTextW(tempramental_string_2);
		}
		if (check_5.GetCheck() > 0)
		{
			lines_quantity.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_5.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			lines_quantity.SetWindowTextW(tempramental_string_2);
		}
		if (check_6.GetCheck() > 0)
		{
			rotation_angle_lines.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_6.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			rotation_angle_lines.SetWindowTextW(tempramental_string_2);
		}
		if (check_7.GetCheck() > 0)
		{
			spray_radius.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_7.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			spray_radius.SetWindowTextW(tempramental_string_2);
		}
		if (check_8.GetCheck() > 0)
		{
			spray_quantity.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_8.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			spray_quantity.SetWindowTextW(tempramental_string_2);
		}
		if (check_9.GetCheck() > 0)
		{
			spray_growing_factor.GetWindowTextW(tempramental_string_1);
			tempramental_double_1 = _wtof(tempramental_string_1);
			edit_9.GetWindowTextW(tempramental_string_2);
			tempramental_double_2 = _wtof(tempramental_string_2);
			tempramental_string_2.Format(CString(L"%.4f"), tempramental_double_1 + tempramental_double_2);
			spray_growing_factor.SetWindowTextW(tempramental_string_2);
		}

		Calculate();
		Plot();
		CString result_file_name = Save();

		Gdiplus::Image* resulting_gif_add = new Gdiplus::Image(result_file_name);

		images.push_back(resulting_gif_add);

		Sleep(1000);
	}
		
	for (auto i = images.begin(); i != images.end(); i++)
	{
		if (*i != nullptr)
		{
			if (i == images.begin())
			{
				resulting_gif = *i;
				parameterValue = Gdiplus::EncoderValueMultiFrame;
				stat = resulting_gif->Save(file_name, &encoderClsid, &encoderParameters);
				if (stat == Gdiplus::Ok)
				{

				}
				else
				{
					//CString r;
					//r.Format(L"EncoderValueMultiFrame error: %d", int(stat));
					//AfxMessageBox(r);
				}
			}
			else
			{
				parameterValue = Gdiplus::EncoderValueFrameDimensionTime;
				stat = resulting_gif->SaveAdd(*i, &encoderParameters);
				if (stat == Gdiplus::Ok)
				{

				}
				else
				{
					//CString r;
					//r.Format(L"EncoderValueFrameDimensionPage error: %d", int(stat));
					//AfxMessageBox(r);
				}
			}
		}
	}
	
	if (resulting_gif != nullptr)
	{
		// Close the multiframe file.
		parameterValue = Gdiplus::EncoderValueFlush;
		stat = resulting_gif->SaveAdd(&encoderParameters);
		if (stat == Gdiplus::Ok)
		{

		}
		else
		{
			//CString r;
			//r.Format(L"EncoderValueFlush error: %d", int(stat));
			//AfxMessageBox(r);
		}
	}

	for (auto i = images.begin(); i != images.end(); i++)
	{
		if (*i != nullptr)
		{
			delete* i;
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}
