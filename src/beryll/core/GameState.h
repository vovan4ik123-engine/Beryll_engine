#pragma once

#include "beryll/core/LayerStack.h"

namespace Beryll
{
    enum class GameStateID
    {
        NO_STATE, // Return from GameStateMachine if states vector is empty.
        UNKNOWN,
        START_SCREEN,
        PLAY,
        PAUSE,
        SETTINGS
    };

    // You can create many objects of this class with different layers in layerStack.
    // Then push these objects to GameStateMachine which will update and draw only last pushed state.
    class GameState final
    {
    public:
        GameState() {}
        ~GameState() {}

        void updateBeforePhysics()
        {
            // start update from last layer = GUI (GUI layer should be pushed as overlay)
            // GUI element can make fingers handled = true. Mean dont handle this finger more on down layers
            for(auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
            {
                (*it)->updateBeforePhysics();
            }
        }

        void updateAfterPhysics()
        {
            // start from first layer
            for(const std::shared_ptr<Layer>& layer : layerStack)
            {
                layer->updateAfterPhysics();
            }
        }

        void draw()
        {
            // start from first layer
            // GUI layer (as overlay) should be drawn last
            for(const std::shared_ptr<Layer>& layer : layerStack)
            {
                layer->draw();
            }
        }

        LayerStack layerStack;
        GameStateID ID = GameStateID::UNKNOWN;
    };
}
