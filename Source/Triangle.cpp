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
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;

	HR(RegisterClassEx(&wc), L"Failed to register class");
	// create window instance;

	m_hwnd = CreateWindowEx(0, pClassName, L"Window", WS_OVERLAPPEDWINDOW, 200, 200, 400, 400, nullptr, nullptr, m_hInstance, nullptr);


	bool sw = ShowWindow(m_hwnd, SW_SHOW);
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

	HR(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_device, &m_featureLevel, &m_immediateContext), L"D3D11CreateDevice Failed.");

	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}
}

void Triangle::CheckMSAAQualityLevel()
{
	HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_m4xMsaaQuality), L"MSAA4x Support check Failed.");
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
	sd.Windowed = TRUE;

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
	sd.OutputWindow = m_hwnd;

	IDXGIDevice* dxgiDevice = nullptr;
	HR((m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**> ( & dxgiDevice))), L"Failed to query DXGI device");

	IDXGIAdapter* dxgiAdapter = nullptr;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**> (&dxgiAdapter)), L"Failed to Get DXGI Adapter");

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory), L"Failed to get DXGI factory pointer");
	
	HR(dxgiFactory->CreateSwapChain(m_device, &sd, &m_SwapChain), L"Failed to create swapchain");


	/*ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);*/
}
