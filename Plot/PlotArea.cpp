#include "pch.h"

#include <time.h>
#include <assert.h>

#include "PlotArea.h"
#include "PlotDlg.h"

#include "SharedConstants.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(PlotArea, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

FLOAT color[] = { 0.25, 0.25, 0.25, 0.0 };

#define PLAY_TIMER_EVENT 1981

HMODULE GetCurrentModule()
{
	HMODULE hModule = nullptr;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}

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

				if (static_scale == 0.0)
				{
					scale = scale_factor * square_size / 2 / absolute_maximum;
				}
				else
				{
					scale = static_scale;
				}

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

					/*///
					CString string;

					string.Format(CString(L"%04d : (%.4f; %.4f)"), int(counter), *list_x_i, *list_y_i);

					auto text_point_y = 12 + 20 + 16 * int(counter);

					if (text_point_y < square_size)
					{
						dc.TextOutW(10, text_point_y, string);
					}
					/*///

					list_x_i++;
					list_y_i++;
				}
			}
		}
	}
}

void PlotArea::OnPaint()
{
	CPaintDC dc(this);
	return;
	{
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
	}
}

CString PlotArea::DrawPicture()
{
	CString result_file_name;

	ShowWindow(SW_HIDE);

	{
		CRect rectangle_2000(0, 0, area_size, area_size);

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

					BOOL result = dc.StretchBlt(0, 0, rectangle_2000.Width(), rectangle_2000.Height(), &cdc, int(x_shift), 0, rectangle.Width() - int(x_shift), rectangle.Height(), SRCCOPY);

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
	}

	ShowWindow(SW_SHOW);

	return result_file_name;
}


int PlotArea::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	//InitWindows();
	InitSwapChain();
	InitRenderTargetView();
	InitShaders();
	InitBuffers();
	InitUAV();
	InitBindings();

	SetTimer(PLAY_TIMER_EVENT, 1000 / 2, nullptr);

	return 0;
}


void PlotArea::Render()
{
	CSingleLock lock(&render_critical_section);

	lock.Lock();

	Frame();
}

void PlotArea::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == PLAY_TIMER_EVENT)
	{
		Render();
	}

	CStatic::OnTimer(nIDEvent);
}


void PlotArea::OnDestroy()
{
	CStatic::OnDestroy();

	KillTimer(PLAY_TIMER_EVENT);

	DisposeUAV();
	DisposeBuffers();
	DisposeShaders();
	DisposeRenderTargetView();
	DisposeSwapChain();
	//DisposeWindows();
}

void PlotArea::InitShaders()
{
	HRESULT result;
	HRSRC src;
	HGLOBAL res;

	if (pDevice != nullptr)
	{
		src = FindResource(GetCurrentModule(), MAKEINTRESOURCE(IDR_BYTECODE_COMPUTE), _T("ShaderObject"));
		res = LoadResource(GetCurrentModule(), src);

		//result = pDevice->CreateComputeShader(
		//	res, SizeofResource(AfxGetInstanceHandle(), src),
		//	nullptr,
		//	&computeShader);
		//assert(SUCCEEDED(result));

		if (res != nullptr)
		{
			FreeResource(res);
		}

		src = FindResource(GetCurrentModule(), MAKEINTRESOURCE(IDR_BYTECODE_PIXEL), _T("ShaderObject"));
		if (src != nullptr)
		{
			res = LoadResource(GetCurrentModule(), src);
			result = pDevice->CreatePixelShader(res, SizeofResource(AfxGetInstanceHandle(), src),
				nullptr, &pixelShader);
			assert(SUCCEEDED(result));

			if (res != nullptr)
			{
				FreeResource(res);
			}
		}

		src = FindResource(GetCurrentModule(), MAKEINTRESOURCE(IDR_BYTECODE_VERTEX), _T("ShaderObject"));
		if (src != nullptr)
		{
			res = LoadResource(GetCurrentModule(), src);
			result = pDevice->CreateVertexShader(res, SizeofResource(AfxGetInstanceHandle(), src),
				nullptr, &vertexShader);
			assert(SUCCEEDED(result));

			D3D11_INPUT_ELEMENT_DESC inputDesc[2];

			inputDesc[0].SemanticName = "POSITION";

			inputDesc[0].SemanticIndex = 0;

			inputDesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;

			inputDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			inputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

			inputDesc[0].InputSlot = 0;

			inputDesc[0].InstanceDataStepRate = 0;

			// Описание второго аргумента функции, аналогично
			inputDesc[1].SemanticName = "COLOR";
			inputDesc[1].SemanticIndex = 0;
			// Цвет --- вектор из трёх 32-битных вещественных чисел
			inputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			// Второй параметр
			inputDesc[1].InputSlot = 1;
			inputDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputDesc[1].InstanceDataStepRate = 0;

			if (src != nullptr)
			{
				result = pDevice->CreateInputLayout(
					inputDesc, 2,
					res, SizeofResource(AfxGetApp()->m_hInstance, src),
					&inputLayout);
				assert(SUCCEEDED(result));

				if (res != nullptr)
				{
					FreeResource(res);
				}
			}
		}
	}
}

void PlotArea::DisposeShaders()
{
	if (inputLayout != nullptr)
	{
		inputLayout->Release();
		inputLayout = nullptr;
	}
	if (computeShader != nullptr)
	{
		computeShader->Release();
		computeShader = nullptr;
	}
	if (vertexShader != nullptr)
	{
		vertexShader->Release();
		vertexShader = nullptr;
	}
	if (pixelShader != nullptr)
	{
		pixelShader->Release();
		pixelShader = nullptr;
	}
}

void PlotArea::InitBuffers()
{
	UINT data_size = UINT(list_x.size());

	HRESULT result;

	float* data = new float[3 * PARTICLE_COUNT];

	if (data != nullptr)
	{
		for (UINT i = 0; i < 3 * PARTICLE_COUNT; i++)
			data[i] = 0.0f;

		D3D11_SUBRESOURCE_DATA subresource;

		subresource.pSysMem = data;

		subresource.SysMemPitch = 0;

		subresource.SysMemSlicePitch = 0;

		D3D11_BUFFER_DESC desc;

		desc.ByteWidth = sizeof(float) * 2 * PARTICLE_COUNT;

		desc.Usage = D3D11_USAGE_DEFAULT;

		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;

		desc.CPUAccessFlags = 0;

		desc.MiscFlags = 0;

		desc.StructureByteStride = 2 * sizeof(float);

		if (list_x.size() == list_y.size() && list_x.size() != 0)
		{
			auto i_x = list_x.begin();
			auto i_y = list_y.begin();

			auto maximum_x_1 = abs(*std::minmax_element(list_x.begin(), list_x.end()).first);
			auto maximum_y_1 = abs(*std::minmax_element(list_y.begin(), list_y.end()).first);
			auto maximum_x_2 = abs(*std::minmax_element(list_x.begin(), list_x.end()).second);
			auto maximum_y_2 = abs(*std::minmax_element(list_y.begin(), list_y.end()).second);

			auto maximum = maximum_x_1;

			if (maximum < maximum_x_2)
			{
				maximum = maximum_x_2;
			}
			if (maximum < maximum_y_1)
			{
				maximum = maximum_y_1;
			}
			if (maximum < maximum_y_2)
			{
				maximum = maximum_y_2;
			}

			for (UINT i = 0; i < data_size; i++)
			{
				float x = 0;
				float y = 0;

				if (i_x != list_x.end())
				{
					x = float(*i_x);
					i_x++;
				}

				if (i_y != list_y.end())
				{
					y = float(*i_y);
					i_y++;
				}

				if (maximum != 0)
				{
					float maximum_float = float(maximum);
					data[2 * i + 0] = x / maximum_float;
					data[2 * i + 1] = y / maximum_float;
				}
			}
		}

		if (pDevice != nullptr)
		{
			result = pDevice->CreateBuffer(&desc, &subresource, &positionBuffer);
			assert(SUCCEEDED(result));

			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

			for (UINT i = 0; i < 2 * data_size; i++)
				data[i] = 0.0f;

			result = pDevice->CreateBuffer(&desc, &subresource, &velocityBuffer);
			assert(SUCCEEDED(result));
		}

		desc.ByteWidth = 3 * PARTICLE_COUNT * sizeof(float);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.StructureByteStride = 3 * sizeof(float);
		auto i_c = point_color.begin();

		for (UINT i = 0; i < 3 * PARTICLE_COUNT; i++)
			data[i] = 0.0f;

		for (UINT i = 0; i < data_size; i++)
		{

			float r = 0;
			float g = 0;
			float b = 0;

			if (i_c != point_color.end())
			{
				r = float(GetRValue(*i_c));
				g = float(GetGValue(*i_c));
				b = float(GetBValue(*i_c));
				i_c++;
			}
			
			float maximum_float = float(256);
			data[3 * i + 0] = r / maximum_float;
			data[3 * i + 1] = g / maximum_float;
			data[3 * i + 2] = b / maximum_float;
		}


		if (pDevice != nullptr)
		{
			result = pDevice->CreateBuffer(&desc, &subresource, &colorBuffer);
			assert(SUCCEEDED(result));
		}

		delete[] data;

		data = nullptr;
	}
}

void PlotArea::DisposeBuffers()
{
	if (positionBuffer != nullptr)
	{
		positionBuffer->Release();
	}
	if (velocityBuffer != nullptr)
	{
		velocityBuffer->Release();
	}
	if (colorBuffer != nullptr)
	{
		colorBuffer->Release();
	}
}

void PlotArea::InitUAV()
{
	HRESULT result;

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.Format = DXGI_FORMAT_R32G32_FLOAT;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	desc.Buffer.FirstElement = 0;

	desc.Buffer.NumElements = PARTICLE_COUNT;

	desc.Buffer.Flags = 0;


	if (pDevice != nullptr)
	{
		result = pDevice->CreateUnorderedAccessView(positionBuffer, &desc,
			&positionUAV);
		assert(!result);

		result = pDevice->CreateUnorderedAccessView(velocityBuffer, &desc,
			&velocityUAV);
		assert(!result);
	}
}

void PlotArea::DisposeUAV()
{
	if (positionUAV != nullptr)
	{
		positionUAV->Release();
		positionUAV = nullptr;
	}
	if (velocityUAV != nullptr)
	{
		velocityUAV->Release();
		velocityUAV = nullptr;
	}
}

void PlotArea::InitBindings()
{
	if (pDeviceContext != nullptr)
	{
		//pDeviceContext->CSSetShader(computeShader, nullptr, 0);

		pDeviceContext->VSSetShader(vertexShader, nullptr, 0);

		pDeviceContext->PSSetShader(pixelShader, nullptr, 0);


		pDeviceContext->CSSetUnorderedAccessViews(1, 1, &velocityUAV, nullptr);

		pDeviceContext->IASetInputLayout(inputLayout);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
}

float PlotArea::AverageFrameTime()
{
	frameTime[currentFrame] = clock();
	int nextFrame = (currentFrame + 1) % FRAME_TIME_COUNT;
	clock_t delta = frameTime[currentFrame] - frameTime[nextFrame];
	currentFrame = nextFrame;
	return (float)delta / CLOCKS_PER_SEC / FRAME_TIME_COUNT;
}

void PlotArea::Frame()
{
	float frameTime = AverageFrameTime();

	//char buf[256];
	//sprintf_s(buf, "Average framerate: %.1f", 1.0f / frameTime);
	//SetWindowTextA(m_hWnd, buf);

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	clearColor[0] = GetRValue(colors[8]) / 256.0f;
	clearColor[1] = GetGValue(colors[8]) / 256.0f;
	clearColor[2] = GetBValue(colors[8]) / 256.0f;
	clearColor[3] = 0.0f;

	if (pDeviceContext != nullptr)
	{
		pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

		UINT stride[] = { sizeof(float[2]), sizeof(float[3]) };
		UINT offset[] = { 0, 0 };

		ID3D11Buffer* nullptrBuffer = nullptr;
		ID3D11UnorderedAccessView* nullptrUAV = nullptr;

		pDeviceContext->IASetVertexBuffers(0, 1, &nullptrBuffer, stride, offset);

		pDeviceContext->CSSetUnorderedAccessViews(0, 1, &positionUAV, nullptr);

		pDeviceContext->Dispatch(PARTICLE_COUNT / NUMTHREADS, 1, 1);

		pDeviceContext->CSSetUnorderedAccessViews(0, 1, &nullptrUAV, nullptr);

		ID3D11Buffer* vertBuffers[] = { positionBuffer, colorBuffer };

		pDeviceContext->IASetVertexBuffers(0, 2, vertBuffers, stride, offset);

		pDeviceContext->Draw(PARTICLE_COUNT, 0);

		if (pSwapChain != nullptr)
		{
			pSwapChain->Present(1, 0);
		}
	}
}

void PlotArea::InitSwapChain()
{
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	swapChainDesc.BufferDesc.Width = 2000;
	swapChainDesc.BufferDesc.Height = 2000;


	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.BufferCount = 1;

	swapChainDesc.OutputWindow = m_hWnd;

	swapChainDesc.Windowed = TRUE;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

#ifndef NDEBUG
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flags = 0;
#endif

	result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr,
		flags,
		&featureLevel, 1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain, &pDevice, nullptr, &pDeviceContext);

	assert(SUCCEEDED(result));
}

void PlotArea::DisposeSwapChain()
{
	pDeviceContext->Release();
	pDevice->Release();
	pSwapChain->Release();
}

void PlotArea::ResizeSwapChain()
{
	HRESULT result;

	RECT rect;

	GetClientRect(&rect);
	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;

	DisposeRenderTargetView();

	if (pSwapChain != nullptr)
	{
		result = pSwapChain->ResizeBuffers(
			0,
			windowWidth, windowHeight,
			DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(result));
	}

	InitRenderTargetView();
}

void PlotArea::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	ResizeSwapChain();
}

void PlotArea::InitRenderTargetView()
{
	HRESULT result;
	ID3D11Texture2D* backBuffer = nullptr;

	if (pSwapChain != nullptr)
	{
		result = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		assert(SUCCEEDED(result));

		if (pDevice != nullptr)
		{
			if (backBuffer != nullptr)
			{
				result = pDevice->CreateRenderTargetView(backBuffer, nullptr, &pRenderTargetView);
				assert(SUCCEEDED(result));

				backBuffer->Release();
				backBuffer = nullptr;

				if (pDeviceContext != nullptr)
				{
					pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

					D3D11_VIEWPORT viewport;
					viewport.TopLeftX = 0;
					viewport.TopLeftY = 0;
					viewport.Width = (FLOAT)windowWidth;
					viewport.Height = (FLOAT)windowHeight;
					viewport.MinDepth = 0;
					viewport.MaxDepth = 1;
					pDeviceContext->RSSetViewports(1, &viewport);
				}
			}
		}
	}
}

void PlotArea::DisposeRenderTargetView()
{
	if (pRenderTargetView != nullptr)
	{
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}
}

