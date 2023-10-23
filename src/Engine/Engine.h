#pragma once

#include <Windows.h>

#include "Input.h"
#include "ApplicationBase.h"

class Engine
{
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindow(int&, int&);
	void ShutdownWindow();

private:
	LPCWSTR m_application_name;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_input;
	ApplicationBase* m_application;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;