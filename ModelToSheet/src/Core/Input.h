#pragma once



// This class is a shell.
// All public methods  call the implementations.
// Implementations are define in Platform specific Input classes
class Input
{
public:
	inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

	inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
	inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
	inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }

protected:
	// Implimentation created per platform - static classes are allowed to access Protected methods
	virtual bool IsKeyPressedImpl(int keycode) = 0;

	virtual bool IsMouseButtonPressedImpl(int button) = 0;
	virtual float GetMouseXImpl() = 0;
	virtual float GetMouseYImpl() = 0;
	virtual std::pair<float, float> GetMousePosImpl() = 0;

private:
	// Singleton
	static Input* s_Instance;
};