#include "LayerStack.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    LayerStack::LayerStack()
    {

    }

    LayerStack::~LayerStack()
    {

    }

    void LayerStack::pushLayer(std::shared_ptr<Layer> layer)
    {
        auto it = std::find_if(m_layers.begin(), m_layers.end(),
                              [&layer](const std::shared_ptr<Layer>& l)
                                    { return l->getLayerID() == layer->getLayerID(); });
        if(it == m_layers.end())
        {

            m_layers.insert(m_layers.begin() + m_layerInsertIndex, layer);
            ++m_layerInsertIndex;
        }
        else
        {
            BR_ASSERT(false, "Dont push layer with same ID again: %d", static_cast<int>(layer->getLayerID()));
        }
    }

    void LayerStack::pushOverlay(std::shared_ptr<Layer> overlay)
    {
        auto it = std::find_if(m_layers.begin(), m_layers.end(),
                              [&overlay](const std::shared_ptr<Layer>& l)
                                    { return l->getLayerID() == overlay->getLayerID(); });
        if(it == m_layers.end())
        {
            m_layers.push_back(overlay);
        }
        else
        {
            BR_ASSERT(false, "Dont push layer with same ID again: %d", static_cast<int>(overlay->getLayerID()));
        }
    }

    void LayerStack::popLayer(LayerID id)
    {
        auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layerInsertIndex,
                              [&id](const std::shared_ptr<Layer>& l)
                                    { return l->getLayerID() == id; });

        if(it != m_layers.begin() + m_layerInsertIndex)
        {
            m_layers.erase(it);
            --m_layerInsertIndex;
        }
    }

    void LayerStack::popOverlay(LayerID id)
    {
        auto it = std::find_if(m_layers.begin() + m_layerInsertIndex, m_layers.end(),
                               [&id](const std::shared_ptr<Layer>& l)
                                     { return l->getLayerID() == id; });

        if(it != m_layers.end())
        {
            m_layers.erase(it);
            --m_layerInsertIndex;
        }
    }

}