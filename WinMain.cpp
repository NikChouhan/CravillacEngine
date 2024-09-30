#include "Triangle.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Triangle triangle(hInstance, 800, 800);

	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	if (gResult == -1)
		return -1;
	else
		return msg.wParam;
}