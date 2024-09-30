#pragma once

#include <Windows.h>
#include <d3d11.h>



class Triangle
{
private:

	HINSTANCE* m_hInstance;
	HWND* m_hwnd;

	UINT m_width = 800;
	UINT m_height = 800;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL m_featureLevel;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_immediateContext;

	bool m_enableMSAA = false;
	UINT m_m4xMsaaQuality;

	DXGI_SWAP_CHAIN_DESC* m_pDesc; // Pointer to swap chain description.
	IDXGISwapChain** m_ppSwapChain;

public:
	void InitWindow(HINSTANCE hinstance, int width, int height);

	Triangle(HINSTANCE hinstance, UINT width, UINT height) : m_hInstance(&hinstance), m_width(width), m_height(height)
	{
		InitWindow(hinstance, width, height);
	}
	void InitDX11();

private:
	void CreateDevice();
	void CheckMSAAQualityLevel();
	void CreateSwapChain();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};