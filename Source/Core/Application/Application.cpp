#include "Application.h"
#include <vector>
#include <iostream>

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	Cravillac::Application* app = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return app->MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Cravillac::Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "Change name");
		}
		break;

	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "Rechange name");
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Cravillac::Application::Application(HINSTANCE hinstance)
	: m_width(1800), m_height(900),
	m_mainWndCaption(L"DX11 Hello World"),
	m_enableMSAA(false), m_hwnd(0), m_appPaused(false),
	m_minimized(false), m_maximized(false),
	m_m4xMsaaQuality(0), m_device(0), m_immediateContext(0),
	m_SwapChain(0), m_depthStencilBuffer(0), m_RenderTargetView(0),
	m_depthStencilView(0)
{
	app = this;
}

Cravillac::Application::~Application()
{
	// Release DirectX resources
	ReleaseCOM(m_depthStencilState);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_immediateContext);
	ReleaseCOM(m_device);
}

int Cravillac::Application::Run()
{
	MSG msg = { 0 };
	m_timer.Reset();
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			m_timer.Tick();
			if (!m_appPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_timer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

HINSTANCE Cravillac::Application::AppInst() const
{
	return m_hInstance;
}

HWND Cravillac::Application::MainWnd() const
{
	return m_hwnd;
}

float Cravillac::Application::AspectRatio() const
{
	return static_cast<float>(m_width) / static_cast<float> (m_height);
}

bool Cravillac::Application::InitWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	HR(RegisterClassW(&wc), L"Failed to create class window");

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_width, m_height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hwnd = CreateWindow(L"D3DWndClassName", m_mainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, 0);
	if (!m_hwnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	return true;
}

void Cravillac::Application::InitDX11()
{
	CreateDevice();
	CheckMSAAQualityLevel();
	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();

	SetViewPort();
}

HRESULT Cravillac::Application::CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !shaderModel)
	{
		return E_INVALIDARG;
	}

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, shaderModel,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}


void Cravillac::Application::CreateDevice()
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

void Cravillac::Application::CheckMSAAQualityLevel()
{
	HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_m4xMsaaQuality), L"MSAA4x Support check Failed.");
	m_enableMSAA = true;
}

void Cravillac::Application::CreateSwapChain()
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
	
	//enumerating devices for fun

	UINT i = 0;
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters;
	std::wstringstream debugOutput;

	debugOutput << L"Enumerating adapters:\n";
	OutputDebugString(L"Total adapters found: ");

	while (dxgiFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		debugOutput << L"Adapter " << i << L": " << adapterDesc.Description << L"\n";
		OutputDebugString(adapterDesc.Description);

		vAdapters.push_back(pAdapter);
		++i;
	}

	
	HR(dxgiFactory->CreateSwapChain(m_device, &sd, &m_SwapChain), L"Failed to create swapchain");


	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}


void Cravillac::Application::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer)), L"Failed to get buffer for render target view");

	HR(m_device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView), L"Failed to create Render Target");

	ReleaseCOM(backBuffer);

}

void Cravillac::Application::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthTextDesc = {};
	depthTextDesc.Width = m_width;
	depthTextDesc.Height = m_height;
	depthTextDesc.MipLevels = 1;
	depthTextDesc.ArraySize = 1;
	depthTextDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_enableMSAA)
	{
		depthTextDesc.SampleDesc.Count = 4;
		depthTextDesc.SampleDesc.Quality = m_m4xMsaaQuality - 1;
	}
	else
	{
		depthTextDesc.SampleDesc.Count = 1;
		depthTextDesc.SampleDesc.Quality = 0;
	}

	depthTextDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextDesc.CPUAccessFlags = 0;
	depthTextDesc.MiscFlags = 0;

	HR(m_device->CreateTexture2D(&depthTextDesc, 0, &m_depthStencilBuffer), L"Failed to create Depth/Stencil buffer");


	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	HR(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState), L"Failed to create depth stencil state");

	// Set the depth stencil state
	m_immediateContext->OMSetDepthStencilState(m_depthStencilState, 1);

	HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView), L"Failed to create Depth/Stencil view");

	//add to immediate context

	m_immediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_depthStencilView);

}

void Cravillac::Application::SetViewPort()
{
	D3D11_VIEWPORT vp;

	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = static_cast<float>(m_width);
	vp.Height = static_cast<float>(m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_immediateContext->RSSetViewports(1, &vp);
}

void Cravillac::Application::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame. These stats 
	// are appeneded to the window caption bar.
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	// Compute averages over one second period.
	if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::wostringstream outs;

		outs.precision(6);
		outs << m_mainWndCaption << L" "
			<< L"FPS: " << fps << L" "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hwnd, outs.str().c_str());
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

bool Cravillac::Application::Init()
{
	if(!InitWindow()) return false;
	InitDX11();

	return true;
}

void Cravillac::Application::OnResize()
{
	assert(m_immediateContext);
	assert(m_device);
	assert(m_SwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(m_SwapChain->ResizeBuffers(1, m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0), L"Failed to resize swap chain buffer");
	ID3D11Texture2D* backBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)), L"Failed to fetch backbuffer for resizing");
	HR(m_device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView), L"Failed to Create RTV for resizing");
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_width;
	depthStencilDesc.Height = m_height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (m_enableMSAA)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(m_device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer), L"Failed to create Depth Stencil buffer for resizing");
	HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView), L"Failed to create DSV for resizing");


	// Bind the render target view and depth/stencil view to the pipeline.

	m_immediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_depthStencilView);


	// Set the viewport transform.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(m_width);
	mScreenViewport.Height = static_cast<float>(m_height);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	m_immediateContext->RSSetViewports(1, &mScreenViewport);
}

LRESULT CALLBACK Cravillac::Application::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPaused = true;
			m_timer.Stop();
		}
		else
		{
			m_appPaused = false;
			m_timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);
		if (m_device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_appPaused = true;
				m_minimized = true;
				m_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_appPaused = false;
				m_minimized = false;
				m_maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (m_minimized)
				{
					m_appPaused = false;
					m_minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_maximized)
				{
					m_appPaused = false;
					m_maximized = false;
					OnResize();
				}
				else if (m_resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_appPaused = true;
		m_resizing = true;
		m_timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_appPaused = false;
		m_resizing = false;
		m_timer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	/*case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;*/
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
