#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class D3DHandler
{
public:
	D3DHandler();
	D3DHandler(const D3DHandler&);
	~D3DHandler();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void SetBackBufferRenderTarget();
	void ResetViewport();

private:
	bool m_vsync_enabled;
	int m_video_card_memory;
	char m_video_card_description[128];
	IDXGISwapChain* m_swap_chain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_device_context;
	ID3D11RenderTargetView* m_render_target_view;
	ID3D11Texture2D* m_depth_stencil_buffer;
	ID3D11DepthStencilState* m_depth_stencil_state;
	ID3D11DepthStencilView* m_depth_stencil_view;
	ID3D11RasterizerState* m_rasterizer_state;
	XMMATRIX m_projection_matrix;
	XMMATRIX m_world_matrix;
	XMMATRIX m_ortho_matrix;
	D3D11_VIEWPORT m_viewport;
};