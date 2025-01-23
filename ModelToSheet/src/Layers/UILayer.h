#pragma once
#include "Layer.h"

class UILayer : public Layer {
public:
	UILayer();
	virtual  ~UILayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnImGuiRender() override;
};