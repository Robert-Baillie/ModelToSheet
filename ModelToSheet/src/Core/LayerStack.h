#include "Layer.h"

#include <vector>


// The LayerStack is essentially a vector of Layer that we loop over.
// Each loop  has different behaviour
// Update -> Loop up through the list
// Events -> Loop down the list.

// The LayerStack has raw pointrs to the layer, we want ownership on this.
// When a layer is pooped it is not destroyed, it still exists and will be destroyed when the app is.

class LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void PopLayer(Layer* layer);
	void PopOverlay(Layer* overlay);

	// Begin and End. Common Vector function wrappers.
	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }


private:
	std::vector<Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};

