#include "../../Source/Core/Application/Application.h"
#include <assert.h>

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

namespace Cravillac
{
	class Cube : public Application
	{
	public:
		Cube(HINSTANCE hInstance);
		~Cube();
		bool Init();
		void OnResize();
		void UpdateScene(float dt);
		void DrawScene();

	private:

		HRESULT CubePipeline();
		HRESULT CreateVertexBuffer();

	private:

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11InputLayout* m_vertexLayout = nullptr;
		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11RasterizerState* m_rasterState = nullptr;

		ID3D11Buffer* m_indexBuffer = nullptr;
		ID3D11Buffer* m_constantBuffer = nullptr;

		XMMATRIX m_world;
		XMMATRIX m_view;
		XMMATRIX m_projection;

	};

	Cube::Cube(HINSTANCE hInstance)
		: Application(hInstance)
	{
	}
	Cube::~Cube()
	{
	}
	bool Cube::Init()
	{
		if (!Application::Init())
			return false;

		HR(CubePipeline(), L"Failed to create Cube Pipeline");
		return true;
	}
	void Cube::OnResize()
	{
		Application::OnResize();
	}
	void Cube::UpdateScene(float dt)
	{

	}

	HRESULT Cube::CubePipeline()
	{
		// Compile vertex shader
		ID3DBlob* vsBlob = nullptr;
		HR(CompileShader(L"Shaders/Cube/Cube_VS.hlsl", "VSMain", "vs_5_0", &vsBlob), L"Failed compiling vertex shader");

		HR(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader), L"Failed to create Vertex shader");

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);

		//create input layout

		HR(m_device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_vertexLayout), L"Failed to create input layout");

		vsBlob->Release();

		m_immediateContext->IASetInputLayout(m_vertexLayout);

		// Compile pixel shader
		ID3DBlob* psBlob = nullptr;
		HR(CompileShader(L"Shaders/Cube/Cube_PS.hlsl", "PSMain", "ps_5_0", &psBlob), L"Failed compiling pixel shader");

		HR(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader), L"Failed to create pixel shader");
		psBlob->Release();

		HR(CreateVertexBuffer(), L"Failed to create vertex buffer");

		

		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.DepthClipEnable = true;

		// Create the rasterizer state object
		HR(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), L"Failed to create rasterizer state");


		return S_OK;
	}

	HRESULT Cube::CreateVertexBuffer()
	{
		// Create vertex buffer
		SimpleVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		};

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * 8;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = vertices;
		HR(m_device->CreateBuffer(&bd, &InitData, &m_vertexBuffer), L"Failed to create vertex buffer");

		// Set vertex buffer
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		m_immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Create index buffer
		UINT indices[] =
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(UINT) * 36;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;

		HR(m_device->CreateBuffer(&bd, &InitData, &m_indexBuffer), L"Failed to create Index buffer");

		m_immediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set primitive topology
		m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Create the constant buffer
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HR(m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer), L"Failed to create constant buffer");

		m_world = DirectX::XMMatrixIdentity();

		// Initialize the view matrix
		XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
		XMVECTOR At = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_view = DirectX::XMMatrixLookAtLH(Eye, At, Up);

		// Initialize the projection matrix
		m_projection = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV2, m_width / (FLOAT)m_height, 0.01f, 100.0f);

		return S_OK;
	}

	void Cube::DrawScene()
	{
		// Update our time
		static float t = 0.0f;
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;

		m_world = DirectX::XMMatrixRotationY(t);


		assert(m_immediateContext);
		assert(m_SwapChain);
		SetViewPort();
		m_immediateContext->ClearRenderTargetView(m_RenderTargetView, Colors::Black);
		//m_immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//m_immediateContext->OMSetDepthStencilState(m_depthStencilState, 1);

		m_immediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_depthStencilView);

		// Update variables
		//
		ConstantBuffer cb;
		cb.mWorld = DirectX::XMMatrixTranspose(m_world);
		cb.mView = DirectX::XMMatrixTranspose(m_view);
		cb.mProjection = DirectX::XMMatrixTranspose(m_projection);
		m_immediateContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb, 0, 0);

		// Render a Cube
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		m_immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_immediateContext->IASetInputLayout(m_vertexLayout);
		m_immediateContext->RSSetState(m_rasterState);

		// Set the shaders
		m_immediateContext->VSSetShader(m_vertexShader, nullptr, 0);
		m_immediateContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
		m_immediateContext->PSSetShader(m_pixelShader, nullptr, 0);

		//draw the Cube
		m_immediateContext->DrawIndexed(36, 0, 0);

		HR((m_SwapChain->Present(0, 0)), L"Failed to draw scene");
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Cravillac::Cube theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
};