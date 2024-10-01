#include "Triangle.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(0, L"Failed to initialize COM.", L"Error", MB_OK);
		return -1;
	}

	Triangle triangle(800, 800);	

	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	CoUninitialize();

	if (gResult == -1)
		return -1;
	else
		return msg.wParam;
}