#pragma once

#include <afxwin.h>
#include <list>

class PlotArea :
    public CStatic
{
protected:
    std::list<double> list_x;
    std::list<double> list_y;

    std::vector<COLORREF> point_color;

    int area_size;

    double scale;
    double scale_factor;
    double static_scale;

    HBITMAP bitmap;

    HBITMAP file_bitmap;

    CImage image;

    std::vector<COLORREF> colors;

    DECLARE_MESSAGE_MAP()

    afx_msg void OnPaint();

public:

    PlotArea() : CStatic(), show_axes(false), scale(1.0), bitmap(nullptr), file_bitmap(nullptr), in_paint(false), area_size(2000), scale_factor(1.0), static_scale(0.0)
    {
    }

    ~PlotArea()
    {
        if (bitmap != nullptr)
        {
            DeleteObject(bitmap);

            bitmap = nullptr;
        }
    }

    void SetLists(const std::list<double> &list_x_p, const std::list<double> &list_y_p, const std::vector<COLORREF> &point_color_p)
    {
        list_x = list_x_p;
        list_y = list_y_p;
        point_color = point_color_p;
    }

    void SetColors(std::vector<COLORREF> colors_p)
    {
        colors = colors_p;
    }

    void SetAreaSize(int area_size_p)
    {
        area_size = area_size_p;
    }

    void SetScaleFactor(double scale_factor_p)
    {
        scale_factor = scale_factor_p;
    }

    void SetStaticScale(double static_scale_p)
    {
        static_scale = static_scale_p;
    }

    const double x_shift = 200.0;

    bool show_axes;

    bool in_paint;

    void Plot(CDC &dc, CRect client_rectangle);

    CString DrawPicture();
};

