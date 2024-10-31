#include "pch.h"
#include "WindowsInput.h"

// Platform specific includes #include <GLFW/glfw3.h>

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
	return false;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
	return false;
}

float WindowsInput::GetMouseXImpl()
{
	return 0.0f;
}

float WindowsInput::GetMouseYImpl()
{
	return 0.0f;
}

std::pair<float, float> WindowsInput::GetMousePosImpl()
{
	return std::pair<float, float>();
}
