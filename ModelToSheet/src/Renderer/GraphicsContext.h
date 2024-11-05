#pragma once

// Abstracted class - this is defined by the renderer you would want to use.
// Implement in the renderer classes.
class GraphicsContext {
public:
	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;
};