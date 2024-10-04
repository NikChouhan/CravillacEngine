#include "Source/Application.h"
#include <assert.h>

class Triangle : public Application
{
public:
	Triangle(HINSTANCE hInstance);
	~Triangle();
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Triangle theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
};


Triangle::Triangle(HINSTANCE hInstance)
	: Application(hInstance)
{
}
Triangle::~Triangle()
{
}
bool Triangle::Init()
{
	if (!Application::Init())
		return false;
	return true;
}
void Triangle::OnResize()
{
	Application::OnResize();
}
void Triangle::UpdateScene(float dt)
{
}
void Triangle::DrawScene()
{
	assert(m_immediateContext);
	assert(m_SwapChain);

	m_immediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Cyan));
	m_immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR((m_SwapChain->Present(0, 0)), L"Failed to draw scene");
}