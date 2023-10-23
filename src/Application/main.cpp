#include "../Engine/Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	Engine* engine;
	bool result;

	engine = new Engine;

	result = engine->Initialize();
	if (result)
		engine->Run();

	engine->Shutdown();
	delete engine;

	return 0;
}