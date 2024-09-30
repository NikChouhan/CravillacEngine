#include "Triangle.h"

LRESULT CALLBACK Triangle::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "Fuck you mate");
		}
		break;

	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "NO pls don't ruin yourself Good Mr");
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



void Triangle::InitWindow(HINSTANCE hinstance, int width, int height)
{
	const auto pClassName = L"hw3d";

	WNDCLASSEX wc{ 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = *m_hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;

	RegisterClassEx(&wc);
	// create window instance;

	HWND hWnd = CreateWindowEx(0, pClassName, L"Window", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 400, 400, nullptr, nullptr, *m_hInstance, nullptr);


	bool sw = ShowWindow(hWnd, SW_SHOW);
}

void Triangle::InitDX11()
{
	CreateDevice();
	CheckMSAAQualityLevel();
	CreateSwapChain();
}

void Triangle::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_device, &m_featureLevel, &m_immediateContext);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
	}
	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}
}

void Triangle::CheckMSAAQualityLevel()
{
	HRESULT hr = m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_m4xMsaaQuality);
	if (FAILED(hr))
	{
		MessageBox(0, L"MSAA4x Support check Failed.", 0, 0);
		return;
	}
	m_enableMSAA = true;
}

void Triangle::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (m_enableMSAA)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = *m_hwnd;
}
