#include "Events/Event.h"


// This is an abstracted class.
// Platform specific classes handle the logic.

struct WindowProps {
	std::string Title;
	unsigned int Width;
	unsigned int Height;
	bool Fullscreen;

	WindowProps(const std::string& title = "Model To Sheet",
		unsigned int width = 1280,
		unsigned int height = 720,
		bool fullscreen = false)
		: Title(title), Width(width), Height(height), Fullscreen(fullscreen) {}
};

class Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() {}

	virtual void OnUpdate() = 0;
	
	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	// Window Attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;
	virtual float GetDeltaTime()  = 0;

	// Get the native Window from GLFW or any other platform specific library.
	virtual void* GetNativeWindow() const = 0;

	// Implement below per platform
	static Window* Create(const WindowProps& props = WindowProps());
};