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

    double scale;

    HBITMAP bitmap;

    HBITMAP file_bitmap;

    CImage image;

    std::vector<COLORREF> colors;

    DECLARE_MESSAGE_MAP()

    afx_msg void OnPaint();

public:

    PlotArea() : CStatic(), show_axes(false), scale(1.0), bitmap(nullptr), in_paint(false)
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

    void SetLists(std::list<double> &list_x_p, std::list<double> &list_y_p, std::vector<COLORREF> &point_color_p)
    {
        list_x = list_x_p;
        list_y = list_y_p;
        point_color = point_color_p;
    }

    void SetColors(std::vector<COLORREF> & colors_p)
    {
        colors = colors_p;
    }

    const double x_shift = 200.0;

    bool show_axes;

    bool in_paint;

    void Plot(CDC &dc, CRect client_rectangle);

    CString DrawPicture();
};

