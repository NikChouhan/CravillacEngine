#include "Source/Application.h"
#include <assert.h>
#include <d3dcompiler.h>

struct SimpleVertex
{
	XMFLOAT3 Pos;
};

class Triangle : public Application
{
public:
	Triangle(HINSTANCE hInstance);
	~Triangle();
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	HRESULT CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** shaderBlob);

private:

	HRESULT TrianglePipeline();
	HRESULT CreateVertexBuffer();

private:

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_vertexLayout = nullptr;
	ID3D11Buffer* m_vertexBuffer = nullptr;
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

	HR(TrianglePipeline(), L"Failed to create Triangle Pipeline");
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

	m_immediateContext->ClearRenderTargetView(m_RenderTargetView, Colors::Red); 

	// Render a triangle

	// Set the shaders
	m_immediateContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_immediateContext->PSSetShader(m_pixelShader, nullptr, 0);

	//draw the triangle

	m_immediateContext->Draw(3, 0);

	HR((m_SwapChain->Present(0, 0)), L"Failed to draw scene");
}
HRESULT Triangle::CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
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

HRESULT Triangle::TrianglePipeline()
{
	// Compile vertex shader
	ID3DBlob* vsBlob = nullptr;
	HR(CompileShader(L"Shaders/Triangle_VS.hlsl", "VSMain", "vs_4_0", &vsBlob), L"Failed compiling vertex shader");

	HR(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader), L"Failed to create Vertex shader");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//create input layout

	HR(m_device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_vertexLayout), L"Failed to create input layout");

	vsBlob->Release();

	m_immediateContext->IASetInputLayout(m_vertexLayout), L"Failed to set vertex layout";


	// Compile pixel shader
	ID3DBlob* psBlob = nullptr;
	HR(CompileShader(L"Shaders/Triangle_PS.hlsl", "PSMain", "ps_4_0", &psBlob), L"Failed compiling pixel shader");

	HR(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader), L"Failed to create pixel shader");
	psBlob->Release();

	HR(CreateVertexBuffer(), L"Failed to create vertex buffer");

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set primitive topology
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

HRESULT Triangle::CreateVertexBuffer()
{
	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		XMFLOAT3(0.0f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	HR(m_device->CreateBuffer(&bd, &InitData, &m_vertexBuffer), L"Failed to create vertex buffer");

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set primitive topology
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}
