#include "Input.h"

Input::Input() {}

Input::Input(const Input& other) {}

Input::~Input() {}

void Input::Initialize() {
	int i;

	for (i = 0; i < 256; i++)
		m_keys[i] = false;

	return;
}

void Input::KeyDown(unsigned int i) {
	m_keys[i] = true;
	return;
}

void Input::KeyUp(unsigned int i) {
	m_keys[i] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key) {
	return m_keys[key];
}