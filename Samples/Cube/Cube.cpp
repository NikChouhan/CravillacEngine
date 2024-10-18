#include "../../Source/Core/Application/Application.h"
#include <assert.h>
#include <DirectXColors.h>
#include "../../Source/Core/Graphics/Camera.h"
#include "../../Source/Core/WinUtil.h"

using namespace DirectX;

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

		wrl::ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
		wrl::ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> m_rasterState = nullptr;

		wrl::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;

		XMMATRIX m_world;
		XMMATRIX m_view;
		XMMATRIX m_projection;

		Graphics::Camera m_camera;
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
		m_camera.InitAsPerspective(90, m_width, m_height);
		m_camera.SetPosition({ 0.0f, 2.0f, -3.5f });
		return true;
	}
	void Cube::OnResize()
	{
		Application::OnResize();
	}
	void Cube::UpdateScene(float dt)
	{
		static float angle = 0.0f;
		angle += dt;

		ConstantBuffer cb = {
			.mWorld = SM::Matrix::CreateRotationY(DirectX::XMScalarCos(angle)) *SM::Matrix::CreateRotationZ(DirectX::XMScalarSin(angle)),
			.mView = m_camera.GetViewMatrix().Transpose(),
			.mProjection = m_camera.GetProjectionMatrix().Transpose()
		};

		m_immediateContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
		m_world = DirectX::XMMatrixRotationY(angle);

	}

	HRESULT Cube::CubePipeline()
	{
		// Compile vertex shader
		ID3DBlob* vsBlob = nullptr;
		HR(CompileShader(L"Shaders/Cube/Cube_VS.hlsl", "VSMain", "vs_4_0", &vsBlob), L"Failed compiling vertex shader");

		HR(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf()), L"Failed to create Vertex shader");

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);

		//create input layout

		HR(m_device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_vertexLayout.GetAddressOf()), L"Failed to create input layout");

		vsBlob->Release();

		m_immediateContext->IASetInputLayout(m_vertexLayout.Get());

		// Compile pixel shader
		ID3DBlob* psBlob = nullptr;
		HR(CompileShader(L"Shaders/Cube/Cube_PS.hlsl", "PSMain", "ps_4_0", &psBlob), L"Failed compiling pixel shader");

		HR(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf()), L"Failed to create pixel shader");
		psBlob->Release();

		HR(CreateVertexBuffer(), L"Failed to create vertex buffer");

		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.DepthClipEnable = true;

		// Create the rasterizer state object
		HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf()), L"Failed to create rasterizer state");


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
		HR(m_device->CreateBuffer(&bd, &InitData, m_vertexBuffer.GetAddressOf()), L"Failed to create vertex buffer");

		// Set vertex buffer
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		m_immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

		// Create index buffer
		UINT32 indices[] =
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
		bd.ByteWidth = sizeof(UINT32) * 36;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;

		HR(m_device->CreateBuffer(&bd, &InitData, m_indexBuffer.GetAddressOf()), L"Failed to create Index buffer");

		m_immediateContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Set primitive topology
		m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Create the constant buffer
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HR(m_device->CreateBuffer(&bd, nullptr, m_constantBuffer.GetAddressOf()), L"Failed to create constant buffer");

		m_world = DirectX::XMMatrixIdentity();

		return S_OK;
	}

	void Cube::DrawScene()
	{
		assert(m_immediateContext);
		assert(m_SwapChain);
		SetViewPort();
		auto rtv = m_RenderTargetView.Get();
		m_immediateContext->ClearRenderTargetView(rtv, DirectX::Colors::CadetBlue);
		m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_immediateContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

		m_immediateContext->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());



		// Render a Cube
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		m_immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_immediateContext->IASetInputLayout(m_vertexLayout.Get());
		m_immediateContext->RSSetState(m_rasterState.Get());

		// Set the shaders
		m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		m_immediateContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

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