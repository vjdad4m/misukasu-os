#include "D3DHandler.h"

D3DHandler::D3DHandler() {
	m_swap_chain = 0;
	m_device = 0;
	m_device_context = 0;
	m_render_target_view = 0;
	m_depth_stencil_buffer = 0;
	m_depth_stencil_state = 0;
	m_depth_stencil_view = 0;
	m_rasterizer_state = 0;
}

D3DHandler::D3DHandler(const D3DHandler& other) {}

D3DHandler::~D3DHandler() {}

bool D3DHandler::Initialize(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screen_depth, float screen_near) {
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapter_output;
	unsigned int num_modes, i, numerator, denominator;
	unsigned long long string_length;
	DXGI_MODE_DESC* display_mode_list;
	DXGI_ADAPTER_DESC adapter_desc;
	int error;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	D3D_FEATURE_LEVEL feature_level;
	ID3D11Texture2D* back_buffer_ptr;
	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	D3D11_RASTERIZER_DESC rasterizer_desc;
	float field_of_view, screen_aspect;

	m_vsync_enabled = vsync;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	result = adapter->EnumOutputs(0, &adapter_output);
	if (FAILED(result))
		return false;

	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
	if (FAILED(result))
		return false;
	
	display_mode_list = new DXGI_MODE_DESC[num_modes];
	if (!display_mode_list)
		return false;

	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
	if (FAILED(result))
		return false;

	for (i = 0; i < num_modes; i++) {
		if (display_mode_list[i].Width == (unsigned int)screen_width) {
			if (display_mode_list[i].Height == (unsigned int)screen_height) {
				numerator = display_mode_list[i].RefreshRate.Numerator;
				denominator = display_mode_list[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapter_desc);
	if (FAILED(result))
		return false;

	m_video_card_memory = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	error = wcstombs_s(&string_length, m_video_card_description, 128, adapter_desc.Description, 128);
	if (error != 0)
		return false;

	delete[] display_mode_list;
	adapter_output->Release();
	adapter->Release();
	factory->Release();

	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = screen_width;
	swap_chain_desc.BufferDesc.Height = screen_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_vsync_enabled) {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;

	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	if (fullscreen) {
		swap_chain_desc.Windowed = false;
	}
	else {
		swap_chain_desc.Windowed = true;
	}

	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

	feature_level = D3D_FEATURE_LEVEL_11_0;

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc, &m_swap_chain, &m_device, NULL, &m_device_context);
	if (FAILED(result))
		return false;

	result = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
	if (FAILED(result))
		return false;

	result = m_device->CreateRenderTargetView(back_buffer_ptr, NULL, &m_render_target_view);
	if (FAILED(result))
		return false;

	back_buffer_ptr->Release();

	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

	depth_buffer_desc.Width = screen_width;
	depth_buffer_desc.Height = screen_height;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;

	result = m_device->CreateTexture2D(&depth_buffer_desc, NULL, &m_depth_stencil_buffer);
	if (FAILED(result))
		return false;

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depth_stencil_desc, &m_depth_stencil_state);
	if (FAILED(result))
		return false;

	m_device_context->OMSetDepthStencilState(m_depth_stencil_state, 1);

	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depth_stencil_buffer, &depth_stencil_view_desc, &m_depth_stencil_view);
	if (FAILED(result))
		return false;

	m_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);

	rasterizer_desc.AntialiasedLineEnable = false;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;

	result = m_device->CreateRasterizerState(&rasterizer_desc, &m_rasterizer_state);
	if (FAILED(result))
		return false;

	m_device_context->RSSetState(m_rasterizer_state);

	m_viewport.Width = (float)screen_width;
	m_viewport.Height = (float)screen_height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_device_context->RSSetViewports(1, &m_viewport);

	field_of_view = 3.141592654f / 4.0f;
	screen_aspect = (float)screen_width / (float)screen_height;

	m_projection_matrix = XMMatrixPerspectiveFovLH(field_of_view, screen_aspect, screen_near, screen_depth);

	m_world_matrix = XMMatrixIdentity();

	m_ortho_matrix = XMMatrixOrthographicLH((float)screen_width, (float)screen_height, screen_near, screen_depth);

	return true;
}

void D3DHandler::Shutdown() {
	if (m_swap_chain)
		m_swap_chain->SetFullscreenState(false, NULL);

	if (m_rasterizer_state)
		m_rasterizer_state->Release();

	if (m_depth_stencil_view)
		m_depth_stencil_view->Release();

	if (m_depth_stencil_state)
		m_depth_stencil_state->Release();

	if (m_depth_stencil_buffer)
		m_depth_stencil_buffer->Release();

	if (m_render_target_view)
		m_render_target_view->Release();

	if (m_device_context)
		m_device_context->Release();

	if (m_device)
		m_device->Release();

	if (m_swap_chain)
		m_swap_chain->Release();
		
	return;
}

void D3DHandler::BeginScene(float red, float green, float blue, float alpha) {
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	m_device_context->ClearRenderTargetView(m_render_target_view, color);
	m_device_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DHandler::EndScene() {
	if (m_vsync_enabled) {
		m_swap_chain->Present(1, 0);
	}
	else {
		m_swap_chain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DHandler::GetDevice() {
	return m_device;
}

ID3D11DeviceContext* D3DHandler::GetDeviceContext() {
	return m_device_context;
}

void D3DHandler::GetProjectionMatrix(XMMATRIX& projection_matrix) {
	projection_matrix = m_projection_matrix;
	return;
}

void D3DHandler::GetWorldMatrix(XMMATRIX& world_matrix) {
	world_matrix = m_world_matrix;
	return;
}

void D3DHandler::GetOrthoMatrix(XMMATRIX& ortho_matrix) {
	ortho_matrix = m_ortho_matrix;
	return;
}

void D3DHandler::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_video_card_description);
	memory = m_video_card_memory;
	return;
}

void D3DHandler::SetBackBufferRenderTarget() {
	m_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);
	return;
}

void D3DHandler::ResetViewport() {
	m_device_context->RSSetViewports(1, &m_viewport);
	return;
}
