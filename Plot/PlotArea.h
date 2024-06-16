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

    ID3D11Device* pDevice;
    ID3D11DeviceContext* pDeviceContext;
    IDXGISwapChain* pSwapChain;
    ID3D11RenderTargetView* pRenderTargetView;

    ID3D11ComputeShader* computeShader;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3D11InputLayout* inputLayout;

    ID3D11Buffer* positionBuffer;
    ID3D11Buffer* velocityBuffer;
    ID3D11Buffer* colorBuffer;

    ID3D11UnorderedAccessView* positionUAV;
    ID3D11UnorderedAccessView* velocityUAV;

    CCriticalSection render_critical_section;

    void InitSwapChain();
    void DisposeSwapChain();
    void InitRenderTargetView();
    void DisposeRenderTargetView();
    void InitShaders();
    void DisposeShaders();
    void InitBuffers();
    void DisposeBuffers();
    void InitUAV();
    void DisposeUAV();
    void InitBindings();

    void ResizeSwapChain();

    static const int FRAME_TIME_COUNT = 128;
    clock_t frameTime[FRAME_TIME_COUNT];
    int currentFrame;

    float AverageFrameTime();
    void Frame();

    int windowWidth, windowHeight;


public:

    PlotArea() : CStatic(), show_axes(false), scale(1.0), bitmap(nullptr), file_bitmap(nullptr), in_paint(false), area_size(2000), scale_factor(1.0), static_scale(0.0)
    {
        pDevice = nullptr;
        pDeviceContext = nullptr;
        pSwapChain = nullptr;
        pRenderTargetView = nullptr;

        computeShader = nullptr;
        vertexShader = nullptr;
        pixelShader = nullptr;

        inputLayout = nullptr;

        positionBuffer = nullptr;
        velocityBuffer = nullptr;
        colorBuffer = nullptr;

        positionUAV = nullptr;
        velocityUAV = nullptr;

        windowWidth = windowHeight = 800;

        currentFrame = 0;

        for (int counter = 0; counter < FRAME_TIME_COUNT;counter++)
        {
            frameTime[counter] = clock_t();
        }
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

    //const double x_shift = 200.0;
    const double x_shift = 0.0;

    bool show_axes;

    bool in_paint;

    void Plot(CDC &dc, CRect client_rectangle);

    CString DrawPicture();

    void Render();

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

