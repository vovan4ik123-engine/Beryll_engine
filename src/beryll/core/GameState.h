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
        SELECT_LEVEL
    };

    inline std::vector<std::string> gameStateAsString{"NO_STATE", "UNKNOWN", "START_SCREEN", "PLAY", "PAUSE", "SETTINGS", "SELECT_LEVEL"};

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
            for(auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
            {
                (*it)->updateAfterPhysics();
            }
        }

        void draw()
        {
            // Start draw from last layer = GUI (GUI layer should be pushed as overlay).
            // This probably avoid draw part of scene hidden by GUI.
            for(auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
            {
                (*it)->draw();
            }
        }

        LayerStack layerStack;
        GameStateID ID = GameStateID::UNKNOWN;
    };
}
