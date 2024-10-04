#pragma once

#include "WinUtil.h"
#include "Timer.h"
#include <string>
#include <sstream>


#define HR(x, errorMsg) \
{ \
    HRESULT hr = (x); \
    if (FAILED(hr)) \
    { \
        MessageBox(0, errorMsg, L"Error", MB_OK); \
		exit(1);	\
    } \
}

#define ReleaseCOM(x) { if(x){ x->Release(); x = nullptr; } }



class Application
{
private:

	HINSTANCE m_hInstance;

	HWND m_hwnd;

	bool m_appPaused; // is the application paused?
	bool m_minimized; // is the application minimized? 
	bool m_maximized; // is the application maximized?
	bool m_resizing; // are the resize bars being dragged?

	UINT m_width = 800;
	UINT m_height = 800;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL m_featureLevel;
protected:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_immediateContext;

	bool m_enableMSAA = false;
	UINT m_m4xMsaaQuality;

	//DXGI_SWAP_CHAIN_DESC* m_pDesc; // Pointer to swap chain description.
	IDXGISwapChain* m_SwapChain;

	ID3D11RenderTargetView* m_RenderTargetView;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;

	D3D11_VIEWPORT mScreenViewport;


	GameTimer m_timer;


	// Window title/caption. D3DApp defaults to "D3D11 Application".
	std::wstring m_mainWndCaption;

public:

	//public member variables


public:
	bool InitWindow();
	void InitDX11();
	Application(HINSTANCE hinstance);

	virtual ~Application();
	int Run();

	//handle and getter functions
	HINSTANCE AppInst()const;
	HWND MainWnd()const;
	float AspectRatio()const;


	void CalculateFrameStats();

	//virtual functions for derived demo classes like triangle and Cube or any demos

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

private:
	void CreateDevice();
	void CheckMSAAQualityLevel();
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();	
	void SetViewPort();
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};