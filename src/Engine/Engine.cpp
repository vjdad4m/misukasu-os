#include "Engine.h"

Engine::Engine() {
	m_input = 0;
	m_application = 0;
}

Engine::Engine(const Engine& other) {}

Engine::~Engine() {}

bool Engine::Initialize() {
	int screen_width, screen_height;
	bool result;

	screen_width = 0;
	screen_height = 0;

	InitializeWindow(screen_width, screen_height);

	m_input = new Input;
	m_input->Initialize();

	m_application = new ApplicationBase;
	
	result = m_application->Initialize(screen_width, screen_height, m_hwnd);
	if (!result)
		return false;

	return true;
}

void Engine::Shutdown() {
	if (m_application) {
		m_application->Shutdown();
		delete m_application;
	}

	if (m_input) {
		delete m_input;
	}

	ShutdownWindow();

	return;
}

void Engine::Run() {
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			done = true;
		else {
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}

	return;
}

bool Engine::Frame() {
	bool result;

	if (m_input->IsKeyDown(VK_ESCAPE))
		return false;

	result = m_application->Frame();
	if (!result)
		return false;

	return true;
}

LRESULT CALLBACK Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_KEYDOWN:
	{
		m_input->KeyDown((unsigned int)wparam);
		return 0;
	}
	case WM_KEYUP:
	{
		m_input->KeyUp((unsigned int)wparam);
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void Engine::InitializeWindow(int& screen_width, int& screen_height) {
	WNDCLASSEX wc;
	DEVMODE dm_screen_settings;
	int pos_x, pos_y;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_application_name = L"Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_application_name;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN) {
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth = (unsigned long)screen_width;
		dm_screen_settings.dmPelsHeight = (unsigned long)screen_height;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);

		pos_x = pos_y = 0;
	} else {
		screen_width = 800;
		screen_height = 600;

		pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
		pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_application_name, m_application_name, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, pos_x, pos_y, screen_width, screen_height, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(false);

	return;
}

void Engine::ShutdownWindow() {
	ShowCursor(true);

	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_application_name, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}