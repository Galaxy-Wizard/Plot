#include "pch.h"
#include "PlotArea.h"

BEGIN_MESSAGE_MAP(PlotArea, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void PlotArea::Plot(CDC& dc, CRect client_rectangle)
{
	if (list_x.size() != 0)
	{
		if (list_x.size() == list_y.size())
		{
			size_t size = list_x.size();

			auto list_x_i = list_x.begin();
			auto list_y_i = list_y.begin();
			
			double center_x = x_shift + (client_rectangle.Width() - x_shift) / 2.0;
			double center_y = client_rectangle.Height() / 2.0;

			double center_x_zeroed = center_x - x_shift;
			double center_y_zeroed = center_y;

			double maximum_x(0.0);
			double minimum_x(0.0);

			for (auto l_x_i = list_x.begin(); l_x_i != list_x.end(); l_x_i++)
			{
				if (maximum_x < *l_x_i)
				{
					maximum_x = *l_x_i;
				}

				if (minimum_x > *l_x_i)
				{
					minimum_x = *l_x_i;
				}
			}

			double maximum_y(0.0);
			double minimum_y(0.0);

			for (auto l_y_i = list_y.begin(); l_y_i != list_y.end(); l_y_i++)
			{
				if (maximum_y < *l_y_i)
				{
					maximum_y = *l_y_i;
				}

				if (minimum_y > *l_y_i)
				{
					minimum_y = *l_y_i;
				}
			}

			double absolute_maximum_x(0.0);

			if (abs(minimum_x) > abs(maximum_x))
			{
				absolute_maximum_x = abs(minimum_x);
			}
			else
			{
				absolute_maximum_x = abs(maximum_x);
			}

			double absolute_maximum_y(0.0);

			if (abs(minimum_y) > abs(maximum_y))
			{
				absolute_maximum_y = abs(minimum_y);
			}
			else
			{
				absolute_maximum_y = abs(maximum_y);
			}

			double absolute_maximum(0.0);

			if (absolute_maximum_x > absolute_maximum_y)
			{
				absolute_maximum = absolute_maximum_x;
			}
			else
			{
				absolute_maximum = absolute_maximum_y;
			}

			if (absolute_maximum != 0.0)
			{
				double square_size(0.0);

				if (center_x_zeroed > center_y_zeroed)
				{
					square_size = center_y_zeroed * 2.0;
				}
				else
				{
					square_size = center_x_zeroed * 2.0;
				}

				scale = square_size / absolute_maximum;

				{
					CRect rectangle
					(
						int(center_x - square_size / 2), int(center_y - square_size / 2),
						int(center_x + square_size / 2), int(center_y + square_size / 2)
					);

					CBrush brush(colors[8]);

					//dc.Draw3dRect(rectangle, colors[8], colors[8]);

					//dc.FillRect(rectangle, &brush);



					TRIVERTEX        vertex[2];
					GRADIENT_RECT    vertexes_rectangle;
					vertex[0].x = rectangle.left;
					vertex[0].y = rectangle.top;
					vertex[0].Red = GetRValue(colors[8]) << 8;
					vertex[0].Green = GetGValue(colors[8]) << 8;
					vertex[0].Blue = GetBValue(colors[8]) << 8;
					vertex[0].Alpha = 0x00;

					vertex[1].x = rectangle.right;
					vertex[1].y = rectangle.bottom;
					vertex[1].Red = GetRValue(colors[9]) << 8;
					vertex[1].Green = GetGValue(colors[9]) << 8;
					vertex[1].Blue = GetBValue(colors[9]) << 8;
					vertex[1].Alpha = 0x00;

					vertexes_rectangle.UpperLeft = 0;
					vertexes_rectangle.LowerRight = 1;
					dc.GradientFill(vertex, 2, &vertexes_rectangle, 1, GRADIENT_FILL_RECT_H);
				}

				if (show_axes)
				{
					dc.MoveTo(CPoint(int(x_shift) + int(0), int(center_y)));
					BOOL relult_1 = dc.LineTo(CPoint(int(x_shift) + int(square_size), int(center_y)));

					dc.MoveTo(CPoint(int(center_x), int(0)));
					BOOL relult_2 = dc.LineTo(CPoint(int(center_x), int(square_size)));
				}

				dc.SetBkColor(colors[10]);

				CString string;

				string.Format(CString(L"Scale: %.4f"), scale);

				dc.TextOutW(10, 12, string);

				for (size_t counter = 0; counter < size; counter++)
				{
					CPoint point_zeroed(int(center_x_zeroed + scale * (*list_x_i)), int(center_y_zeroed - scale * (*list_y_i)));

					auto half_square = int(square_size) / 2;

					if (
						(point_zeroed.x > int(center_x_zeroed) - half_square) && (point_zeroed.x < int(center_x_zeroed) + half_square)
						&&
						(point_zeroed.y > int(center_y_zeroed) - half_square) && (point_zeroed.y < int(center_y_zeroed) + half_square))
					{
						auto point(point_zeroed);

						point.x += LONG(x_shift);
						point.y += 0;

						dc.SetPixel(point, point_color.at(counter));
					}


					CString string;

					string.Format(CString(L"%04d : (%.4f; %.4f)"), int(counter), *list_x_i, *list_y_i);

					auto text_point_y = 12 + 20 + 16 * int(counter);

					if (text_point_y < square_size)
					{
						dc.TextOutW(10, text_point_y, string);
					}

					list_x_i++;
					list_y_i++;
				}
			}
		}
	}
}

void PlotArea::OnPaint()
{
	//if (!in_paint)
	{
		//in_paint = true;

		CRect rectangle;

		GetClientRect(&rectangle);
		
		CPaintDC dc(this);

		{
			CDC cdc;

			BOOL result = cdc.CreateCompatibleDC(&dc);

			if (result == TRUE)
			{
				if (bitmap != nullptr)
				{
					DeleteObject(bitmap);

					bitmap = nullptr;
				}

				bitmap = CreateCompatibleBitmap(dc, rectangle.Width(), rectangle.Height());

				if (bitmap != nullptr)
				{
					HBITMAP old_bitmap = (HBITMAP)cdc.SelectObject(bitmap);

					{
						CBrush white_brush(colors[10]);

						HBITMAP old_brush = (HBITMAP)cdc.SelectObject(white_brush);

						cdc.FillRect(&rectangle, &white_brush);

						SelectObject(cdc, old_brush);
					}

					Plot(cdc, rectangle);

					BOOL result = dc.BitBlt(0, 0, rectangle.Width(), rectangle.Height(), &cdc, 0, 0, SRCCOPY);
										

					SelectObject(cdc, old_bitmap);

					DeleteObject(bitmap);

					bitmap = nullptr;
				}
			}
		}

		//in_paint = false;
	}

}

CString PlotArea::DrawPicture()
{
	CString result_file_name;

	ShowWindow(SW_HIDE);

	//if (!in_paint)
	{
		//in_paint = true;

		CRect rectangle_2000(0, 0, 2000, 2000);

		CRect rectangle;

		GetClientRect(&rectangle);

		CPaintDC dc(this);

		{
			CDC cdc;

			BOOL result = cdc.CreateCompatibleDC(&dc);

			if (result == TRUE)
			{
				if (bitmap != nullptr)
				{
					DeleteObject(bitmap);

					bitmap = nullptr;
				}

				bitmap = CreateCompatibleBitmap(dc, rectangle.Width(), rectangle.Height());

				if (bitmap != nullptr)
				{
					HBITMAP old_bitmap = (HBITMAP)cdc.SelectObject(bitmap);

					{
						CBrush white_brush(colors[10]);

						HBITMAP old_brush = (HBITMAP)cdc.SelectObject(white_brush);

						cdc.FillRect(&rectangle, &white_brush);

						SelectObject(cdc, old_brush);
					}

					Plot(cdc, rectangle);

					BOOL result = dc.StretchBlt(0, 0, rectangle_2000.Width(), rectangle_2000.Height(), &cdc, int(x_shift), 0, rectangle.Width()-int(x_shift), rectangle.Height(), SRCCOPY);

					{
						CDC file_cdc;

						BOOL result = file_cdc.CreateCompatibleDC(&dc);

						if (result == TRUE)
						{

							file_bitmap = CreateCompatibleBitmap(dc, rectangle_2000.Height(), rectangle_2000.Height());

							if (file_bitmap != nullptr)
							{
								HBITMAP old_bitmap = (HBITMAP)file_cdc.SelectObject(file_bitmap);

								{
									CBrush white_brush(colors[10]);

									HBITMAP old_brush = (HBITMAP)file_cdc.SelectObject(white_brush);

									file_cdc.FillRect(&rectangle, &white_brush);

									SelectObject(file_cdc, old_brush);
								}

								BOOL result = file_cdc.StretchBlt(0, 0, rectangle_2000.Height(), rectangle_2000.Height(), &cdc, int(x_shift), 0, rectangle.Width() - int(x_shift), rectangle.Height(), SRCCOPY);


								image.Attach(file_bitmap);

								std::filesystem::path path = std::filesystem::current_path();

								CStringW file_name;

								auto date_time = COleDateTime::GetTickCount();

								file_name.Format(CString(L"\\Plot%4d.%2d.%2d_%2d-%2d-%2d.png"),
									date_time.GetYear(), date_time.GetMonth(), date_time.GetDay(),
									date_time.GetHour(), date_time.GetMinute(), date_time.GetSecond());

								result_file_name = CStringW(path.c_str()) + file_name;

								HRESULT saving_result = image.Save(result_file_name);

								image.Detach();

								DeleteObject(file_bitmap);

								file_bitmap = nullptr;
							}
						}
					}

					SelectObject(cdc, old_bitmap);

					DeleteObject(bitmap);

					bitmap = nullptr;
				}
			}
		}

		//in_paint = false;
	}

	ShowWindow(SW_SHOW);

	return result_file_name;
}