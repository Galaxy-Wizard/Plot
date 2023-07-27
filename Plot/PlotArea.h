#pragma once

#include <afxwin.h>
#include <list>

class PlotArea :
    public CStatic
{
protected:
    std::list<double> list_x;
    std::list<double> list_y;

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
        colors.resize(16);

        colors[0] = RGB(255, 255, 0);
        colors[1] = RGB(0, 0, 0);
        colors[2] = RGB(0, 0, 0);
        colors[3] = RGB(0, 0, 0);

        colors[4] = RGB(0, 0, 0);
        colors[5] = RGB(0, 0, 0);
        colors[6] = RGB(0, 0, 0);
        colors[7] = RGB(0, 0, 0);

        colors[8] = RGB(0, 0, 255);
        colors[9] = RGB(255, 255, 255);
        colors[10] = RGB(0, 0, 0);
        colors[11] = RGB(0, 0, 0);

        colors[12] = RGB(0, 0, 0);
        colors[13] = RGB(0, 0, 0);
        colors[14] = RGB(0, 0, 0);
        colors[15] = RGB(0, 0, 0);
    }

    ~PlotArea()
    {
        if (bitmap != nullptr)
        {
            DeleteObject(bitmap);

            bitmap = nullptr;
        }
    }

    void SetLists(std::list<double> &list_x_p, std::list<double> &list_y_p)
    {
        list_x = list_x_p;
        list_y = list_y_p;
    }

    void SetColors(std::vector<COLORREF> & colors_p)
    {
        colors = colors_p;
    }

    const double x_shift = 200.0;

    bool show_axes;

    bool in_paint;

    void Plot(CDC &dc);

    void DrawPicture();
};

