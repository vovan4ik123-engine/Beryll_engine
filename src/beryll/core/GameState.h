#pragma once

#include "beryll/core/LayerStack.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    enum class GameStateID
    {
        NO_STATE = 0, // Return from GameStateMachine if states vector is empty.
        UNKNOWN,
        START_SCREEN,
        PLAY,
        PAUSE,
        SETTINGS,
        SELECT_LEVEL,
        SHOP,
        PLAYER_TALENTS
    };

    inline const std::vector<const std::string> gameStateAsString{"NO_STATE", "UNKNOWN", "START_SCREEN", "PLAY", "PAUSE", "SETTINGS",
                                                                  "SELECT_LEVEL", "SHOP", "PLAYER_TALENTS"};

    // You can create many objects of this class with different layers in layerStack.
    // Then push these objects to GameStateMachine which will update and draw only last pushed state.
    class GameState final
    {
    public:
        GameState() { BR_INFO("%s", "GameState()"); }
        ~GameState() { BR_INFO("%s", "::~GameState()"); }

        void updateBeforePhysics()
        {
            // Start update from last layer = GUI (GUI layer should be pushed as overlay).
            // GUI element can make fingers handled = true. Mean dont handle this finger more on down layers.
            for(auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
            {
                (*it)->updateBeforePhysics();
            }
        }

        void updateAfterPhysics()
        {
            for(auto it = layerStack.begin(); it != layerStack.end(); ++it)
            {
                (*it)->updateAfterPhysics();
            }
        }

        void draw()
        {
            for(auto it = layerStack.begin(); it != layerStack.end(); ++it)
            {
                (*it)->draw();
            }
        }

        LayerStack layerStack;
        GameStateID ID = GameStateID::UNKNOWN;
    };
}
