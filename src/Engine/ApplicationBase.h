#pragma once

#include <Windows.h>

#include "D3DHandler.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationBase
{
public:
	ApplicationBase();
	ApplicationBase(const ApplicationBase&);
	~ApplicationBase();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DHandler* m_direct_3d;
};