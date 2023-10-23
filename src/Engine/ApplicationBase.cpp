#include "ApplicationBase.h"

ApplicationBase::ApplicationBase() {}

ApplicationBase::ApplicationBase(const ApplicationBase& other) {}

ApplicationBase::~ApplicationBase() {}

bool ApplicationBase::Initialize(int screen_width, int screen_height, HWND hwnd) {
	return true;
}

void ApplicationBase::Shutdown() {
	return;
}

bool ApplicationBase::Frame() {
	return true;
}

bool ApplicationBase::Render() {
	return true;
}