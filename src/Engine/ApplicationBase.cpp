#include "ApplicationBase.h"

ApplicationBase::ApplicationBase() {
	m_direct_3d = 0;
}

ApplicationBase::ApplicationBase(const ApplicationBase& other) {}

ApplicationBase::~ApplicationBase() {}

bool ApplicationBase::Initialize(int screen_width, int screen_height, HWND hwnd) {
	bool result;

	m_direct_3d = new D3DHandler;

	result = m_direct_3d->Initialize(screen_width, screen_height, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationBase::Shutdown() {
	if (m_direct_3d) {
		m_direct_3d->Shutdown();
		delete m_direct_3d;
	}

	return;
}

bool ApplicationBase::Frame() {
	bool result;

	result = Render();
	if (!result)
		return false;

	return true;
}

bool ApplicationBase::Render() {
	m_direct_3d->BeginScene(0.0f, 1.5f, 1.0f, 1.0f);

	m_direct_3d->EndScene();
	
	return true;
}