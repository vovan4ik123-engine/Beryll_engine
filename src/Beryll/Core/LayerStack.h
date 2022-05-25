#pragma once

#include "CppHeaders.h"

#include "Beryll/Core/Layer.h"

namespace Beryll
{
    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void pushLayer(std::shared_ptr<Layer> layer);
        void pushOverlay(std::shared_ptr<Layer> overlay);
        void popLayer(LayerID id);
        void popOverlay(LayerID id);

        std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::iterator end() { return m_layers.end(); }

        std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_layers.rbegin(); }
        std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() { return m_layers.rend(); }

    private:
        std::vector<std::shared_ptr<Layer>> m_layers;
        uint32_t m_layerInsertIndex = 0;
    };
}