
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
//#define new DEBUG_NEW
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
	DDX_Control(pDX, IDC_EDIT_PICTURE_AREA_SIZE, edit_picture_area_size);
	DDX_Control(pDX, IDC_EDIT_A_U, edit_a_u);
	DDX_Control(pDX, IDC_EDIT_B_U, edit_b_v);
	DDX_Control(pDX, IDC_EDIT_U_1, edit_u_1);
	DDX_Control(pDX, IDC_EDIT_U_N, edit_u_n);
	DDX_Control(pDX, IDC_EDIT_U_STEP, edit_u_step);
	DDX_Control(pDX, IDC_EDIT_V_1, edit_v_1);
	DDX_Control(pDX, IDC_EDIT_V_N, edit_v_n);
	DDX_Control(pDX, IDC_EDIT_V_STEP, edit_v_step);

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
	ON_BN_CLICKED(IDC_BUTTON_SAVE_DIGITAL, &CPlotDlg::OnBnClickedButtonSaveDigital)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DIGITAL, &CPlotDlg::OnBnClickedButtonLoadDigital)
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

	X_Expression.addConstant('a', 1);
	Y_Expression.addConstant('a', 1);

	X_Expression.addConstant('b', 1);
	Y_Expression.addConstant('b', 1);

	edit_x_t.SetWindowTextW(L" - 1.0 * t * sin(3.1415 * t / 4.0)");
	edit_y_t.SetWindowTextW(L" 1.0 * t * cos(3.1415 * t / 4.0)");

	formula_is_correct = true;

	A_Expression.setNewXAlias('u');
	B_Expression.setNewXAlias('v');

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

double CPlotDlg::x(double t, double a, double b)
{
	double result(0.0);

	try
	{
		X_Expression.addConstant('a', a);
		X_Expression.addConstant('b', b);

		result = X_Expression.getResult(t);
	}

	//catch (smp::InvalidExpression& exp) //Base kind of any excecption (parent class for all exceptions in smp)
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectSyntax& exp) //That`s gonna throw IncorrectSyntax: x^
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::MathError& exp) //That throwed if you break math rules: (-4) ^ (1 / 4) or 1 / 0
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::RecursionException& exp) //That throwed if your expression uses other expression that has reference to first one
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::InvalidBracketsCount& exp) //That`s gonna throw InvalidBracketsCount: ((2 + 1) * x
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	//catch (smp::MathFunctionCrash& exp) //Common kind of function erros, can be throwed only with inheritor
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectArgument& exp) //That throwed when you use incorrect arguments with function: asin(10) or sqrt(-4)
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::ConversionError& exp) //That`s gonna throw ConversionError: x.23 or x,
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	return result;
}

double CPlotDlg::y(double t, double a, double b)
{
	double result(0.0);

	try
	{
		Y_Expression.addConstant('a', a);
		Y_Expression.addConstant('b', b);

		result = Y_Expression.getResult(t);
	}

	//catch (smp::InvalidExpression& exp) //Base kind of any excecption (parent class for all exceptions in smp)
	//
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectSyntax& exp) //That`s gonna throw IncorrectSyntax: x^
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::MathError& exp) //That throwed if you break math rules: (-4) ^ (1 / 4) or 1 / 0
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::RecursionException& exp) //That throwed if your expression uses other expression that has reference to first one
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::InvalidBracketsCount& exp) //That`s gonna throw InvalidBracketsCount: ((2 + 1) * x
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	//catch (smp::MathFunctionCrash& exp) //Common kind of function erros, can be throwed only with inheritor
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectArgument& exp) //That throwed when you use incorrect arguments with function: asin(10) or sqrt(-4)
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::ConversionError& exp) //That`s gonna throw ConversionError: x.23 or x,
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	return result;
}

double CPlotDlg::a(double u)
{
	double result(0.0);

	try
	{
		result = A_Expression.getResult(u);
	}

	//catch (smp::InvalidExpression& exp) //Base kind of any excecption (parent class for all exceptions in smp)
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectSyntax& exp) //That`s gonna throw IncorrectSyntax: x^
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::MathError& exp) //That throwed if you break math rules: (-4) ^ (1 / 4) or 1 / 0
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::RecursionException& exp) //That throwed if your expression uses other expression that has reference to first one
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::InvalidBracketsCount& exp) //That`s gonna throw InvalidBracketsCount: ((2 + 1) * x
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	//catch (smp::MathFunctionCrash& exp) //Common kind of function erros, can be throwed only with inheritor
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectArgument& exp) //That throwed when you use incorrect arguments with function: asin(10) or sqrt(-4)
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::ConversionError& exp) //That`s gonna throw ConversionError: x.23 or x,
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	return result;
}

double CPlotDlg::b(double v)
{
	double result(0.0);

	try
	{
		result = B_Expression.getResult(v);
	}

	//catch (smp::InvalidExpression& exp) //Base kind of any excecption (parent class for all exceptions in smp)
	//
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectSyntax& exp) //That`s gonna throw IncorrectSyntax: x^
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::MathError& exp) //That throwed if you break math rules: (-4) ^ (1 / 4) or 1 / 0
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::RecursionException& exp) //That throwed if your expression uses other expression that has reference to first one
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::InvalidBracketsCount& exp) //That`s gonna throw InvalidBracketsCount: ((2 + 1) * x
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	//catch (smp::MathFunctionCrash& exp) //Common kind of function erros, can be throwed only with inheritor
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectArgument& exp) //That throwed when you use incorrect arguments with function: asin(10) or sqrt(-4)
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::ConversionError& exp) //That`s gonna throw ConversionError: x.23 or x,
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	return result;
}

void CPlotDlg::OnBnClickedButtonCalculate()
{
	Calculate();
}

void CPlotDlg::Calculate()
{
	formula_is_correct = true;

	CString x_t;
	CString y_t;

	edit_x_t.GetWindowTextW(x_t);
	edit_y_t.GetWindowTextW(y_t);

	CString a_u;
	CString b_v;

	edit_a_u.GetWindowTextW(a_u);
	edit_b_v.GetWindowTextW(b_v);

	try
	{
		X_Expression.setExpression(CStringA(x_t).GetBuffer());
		Y_Expression.setExpression(CStringA(y_t).GetBuffer());

		A_Expression.setExpression(CStringA(a_u).GetBuffer());
		B_Expression.setExpression(CStringA(b_v).GetBuffer());
	}
	//catch (smp::InvalidExpression& exp) //Base kind of any excecption (parent class for all exceptions in smp)
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectSyntax& exp) //That`s gonna throw IncorrectSyntax: x^
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::MathError& exp) //That throwed if you break math rules: (-4) ^ (1 / 4) or 1 / 0
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::RecursionException& exp) //That throwed if your expression uses other expression that has reference to first one
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::InvalidBracketsCount& exp) //That`s gonna throw InvalidBracketsCount: ((2 + 1) * x
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	//catch (smp::MathFunctionCrash& exp) //Common kind of function erros, can be throwed only with inheritor
	//{
	//	formula_is_correct = false;

	//	AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	//}

	catch (smp::IncorrectArgument& exp) //That throwed when you use incorrect arguments with function: asin(10) or sqrt(-4)
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}

	catch (smp::ConversionError& exp) //That`s gonna throw ConversionError: x.23 or x,
	{
		formula_is_correct = false;

		AfxMessageBox(CString("Incorrect expression in formula(s). ") + CString(exp.what()), MB_ICONEXCLAMATION);
	}


	if (!formula_is_correct)
	{
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


	CString starting_value_v_string;
	CString ending_value_v_string;
	CString step_v_string;

	edit_v_1.GetWindowTextW(starting_value_v_string);
	edit_v_n.GetWindowTextW(ending_value_v_string);
	edit_v_step.GetWindowTextW(step_v_string);

	double starting_v(_wtof(starting_value_v_string));
	double ending_v(_wtof(ending_value_v_string));
	double step_v(_wtof(step_v_string));

	if (step_v == 0.0)
	{
	//	return;
	}



	CString starting_value_u_string;
	CString ending_value_u_string;
	CString step_u_string;

	edit_u_1.GetWindowTextW(starting_value_u_string);
	edit_u_n.GetWindowTextW(ending_value_u_string);
	edit_u_step.GetWindowTextW(step_u_string);

	double starting_u(_wtof(starting_value_u_string));
	double ending_u(_wtof(ending_value_u_string));
	double step_u(_wtof(step_u_string));

	if (step_u == 0.0)
	{
	//	return;
	}

	for (double u = starting_u; u < ending_u - step_u * 0.5; u+= step_u)
	{
		double value_a = a(u);

		for (double v = starting_v; v < ending_v - step_v * 0.5; v+= step_v)
		{
			double value_b = b(v);

			for (double t = starting_t; t < ending_t - step_t * 0.5; t += step_t)
			{
				double value_x = x(t, value_a, value_b);

				if (!formula_is_correct)
				{
					list_x.clear();
					list_y.clear();

					point_color.clear();

					return;
				}

				double value_y = y(t, value_a, value_b);

				if (!formula_is_correct)
				{
					list_x.clear();
					list_y.clear();

					point_color.clear();

					return;
				}

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
		}
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
		CString picture_area_size_string;
		edit_picture_area_size.GetWindowTextW(picture_area_size_string);

		int picture_area_size_integer = _wtoi(picture_area_size_string.GetBuffer());

		plot_area.SetAreaSize(picture_area_size_integer);

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

		if (picture_area_size_integer < minimum_square)
		{
			minimum_square = picture_area_size_integer;
		}

		if (plot_area.m_hWnd != nullptr)
		{
			plot_area.DestroyWindow();
			plot_area.m_hWnd = nullptr;
		}
		
		if (
			plot_area.CreateEx(WS_EX_CLIENTEDGE, CString("Static"), CString("Plot area"), WS_CHILD, CRect(int(x_shift), 0, int(minimum_square + plot_area.x_shift + x_shift), int(minimum_square)), this, IDC_STATIC_PLOT_AREA)
			== TRUE
			)
		{
			plot_area.ShowWindow(SW_SHOW);
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
	spray_quantity.SetWindowTextW(CString(L"10.0"));
	spray_growing_factor.SetWindowTextW(CString(L"1619.0"));

	allow_drawing.SetCheck(1);

	CString points_quantity_string;

	points_quantity_string.Format(CString("Points quantity is 2 x %d"), int(list_x.size()));

	points_quantity.SetWindowTextW(points_quantity_string);

	colors.resize(16);

	colors[0] = RGB(255, 255, 0);
	colors[1] = RGB(255, 255, 0);
	colors[2] = RGB(0, 0, 0);
	colors[3] = RGB(0, 0, 0);

	colors[4] = RGB(0, 0, 0);
	colors[5] = RGB(0, 0, 0);
	colors[6] = RGB(0, 0, 0);
	colors[7] = RGB(0, 0, 0);

	colors[8] = RGB(0, 0, 255);
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

	edit_picture_area_size.SetWindowTextW(L"2000");

	edit_a_u.SetWindowTextW(L"1");
	edit_b_v.SetWindowTextW(L"v");

	edit_u_1.SetWindowTextW(L"1");
	edit_u_n.SetWindowTextW(L"1.1");
	edit_u_step.SetWindowTextW(L"0.1");

	edit_v_1.SetWindowTextW(L"1");
	edit_v_n.SetWindowTextW(L"1.1");
	edit_v_step.SetWindowTextW(L"0.1");
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

	Gdiplus::Image* resulting_gif = nullptr;
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
	INT  result = GetEncoderClsid(L"image/gif", &encoderClsid);
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


void CPlotDlg::OnBnClickedButtonSaveDigital()
{
	std::filesystem::path path = std::filesystem::current_path();

	CStringW file_name;

	auto date_time = COleDateTime::GetTickCount();

	file_name.Format(CString(L"\\Plot%4d.%2d.%2d_%2d-%2d-%2d.galaxy_plot"),
		date_time.GetYear(), date_time.GetMonth(), date_time.GetDay(),
		date_time.GetHour(), date_time.GetMinute(), date_time.GetSecond());

	auto result_file_name = CStringW(path.c_str()) + file_name;

	CFileDialog fd(FALSE, L"galaxy_plot", result_file_name);

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	CString file_name_s = fd.GetPathName();

	CStdioFile file_s;

	if (file_s.Open(file_name_s, CStdioFile::modeCreate | CStdioFile::modeWrite | CStdioFile::typeText | CStdioFile::typeUnicode) != TRUE)
	{
		return;
	}

	CString data_string;
	int8_t data_check;
	COLORREF data_color;

	starting_value.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	ending_value.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	step.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	data_check = symmetric_x.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = symmetric_y.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = symmetric_xy.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));

	rotation_angle.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	data_check = lines.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));

	lines_quantity.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	rotation_angle_lines.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	spray_radius.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	spray_quantity.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	spray_growing_factor.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	data_check = allow_drawing.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	
	data_check = check_1.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_2.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_3.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_4.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_5.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_6.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_7.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_8.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));
	data_check = check_9.GetCheck(); file_s.Write(&data_check, sizeof(int8_t));

	edit_1.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_2.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_3.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_4.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_5.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_6.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_7.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_8.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_9.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_10.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	edit_picture_area_size.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	edit_x_t.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_y_t.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	data_color = colors[0]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[1]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[2]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[3]; file_s.Write(&data_color, sizeof(COLORREF));

	data_color = colors[4]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[5]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[6]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[7]; file_s.Write(&data_color, sizeof(COLORREF));

	data_color = colors[8]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[9]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[10]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[11]; file_s.Write(&data_color, sizeof(COLORREF));

	data_color = colors[12]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[13]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[14]; file_s.Write(&data_color, sizeof(COLORREF));
	data_color = colors[15]; file_s.Write(&data_color, sizeof(COLORREF));

	edit_a_u.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_b_v.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	edit_u_1.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_u_n.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_u_step.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	edit_v_1.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_v_n.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");
	edit_v_step.GetWindowTextW(data_string); file_s.WriteString(data_string); file_s.WriteString(L"\n");

	file_s.Close();
}


void CPlotDlg::OnBnClickedButtonLoadDigital()
{
	CFileDialog fd(TRUE);

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	CString file_name = fd.GetPathName();

	CStdioFile file_l;

	if (file_l.Open(file_name, CStdioFile::modeRead | CStdioFile::typeText | CStdioFile::typeUnicode) != TRUE)
	{
		return;
	}

	CString data_string;
	int8_t data_check;
	COLORREF data_color;

	file_l.ReadString(data_string); starting_value.SetWindowTextW(data_string);
	file_l.ReadString(data_string); ending_value.SetWindowTextW(data_string);
	file_l.ReadString(data_string); step.SetWindowTextW(data_string);

	file_l.Read(&data_check, sizeof(int8_t)); symmetric_x.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); symmetric_y.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); symmetric_xy.SetCheck(data_check);

	file_l.ReadString(data_string); rotation_angle.SetWindowTextW(data_string);

	file_l.Read(&data_check, sizeof(int8_t)); lines.SetCheck(data_check);

	file_l.ReadString(data_string); lines_quantity.SetWindowTextW(data_string);
	file_l.ReadString(data_string); rotation_angle_lines.SetWindowTextW(data_string);

	file_l.ReadString(data_string); spray_radius.SetWindowTextW(data_string);
	file_l.ReadString(data_string); spray_quantity.SetWindowTextW(data_string);
	file_l.ReadString(data_string); spray_growing_factor.SetWindowTextW(data_string);

	file_l.Read(&data_check, sizeof(int8_t)); allow_drawing.SetCheck(data_check);

	file_l.Read(&data_check, sizeof(int8_t)); check_1.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_2.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_3.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_4.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_5.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_6.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_7.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_8.SetCheck(data_check);
	file_l.Read(&data_check, sizeof(int8_t)); check_9.SetCheck(data_check);

	file_l.ReadString(data_string); edit_1.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_2.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_3.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_4.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_5.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_6.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_7.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_8.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_9.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_10.SetWindowTextW(data_string);

	file_l.ReadString(data_string); edit_picture_area_size.SetWindowTextW(data_string);

	file_l.ReadString(data_string); edit_x_t.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_y_t.SetWindowTextW(data_string);

	colors.resize(16);

	file_l.Read(&data_color, sizeof(COLORREF)); colors[0] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[1] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[2] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[3] = data_color;

	file_l.Read(&data_color, sizeof(COLORREF)); colors[4] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[5] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[6] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[7] = data_color;

	file_l.Read(&data_color, sizeof(COLORREF)); colors[8] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[9] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[10] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[11] = data_color;

	file_l.Read(&data_color, sizeof(COLORREF)); colors[12] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[13] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[14] = data_color;
	file_l.Read(&data_color, sizeof(COLORREF)); colors[15] = data_color;


	file_l.ReadString(data_string); edit_a_u.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_b_v.SetWindowTextW(data_string);

	file_l.ReadString(data_string); edit_u_1.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_u_n.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_u_step.SetWindowTextW(data_string);

	file_l.ReadString(data_string); edit_v_1.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_v_n.SetWindowTextW(data_string);
	file_l.ReadString(data_string); edit_v_step.SetWindowTextW(data_string);

	file_l.Close();

	plot_area.SetColors(colors);
}
