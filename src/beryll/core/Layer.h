#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    enum class LayerID
    {
        UNKNOWN,
        START_SCREEN_GUI,
        PLAY_GUI, PLAY_SCENE,
        PAUSE_GUI,
        SETTINGS_GUI,
        SELECT_MAP_GUI, SELECT_LEVEL_GUI, SELECT_STONES_GUI
    };

    class Layer
    {
    public:
        virtual ~Layer() {}

        virtual void updateBeforePhysics() = 0; // Handle users input, move objects here.
        virtual void updateAfterPhysics() = 0; // Update positions after simulation, resolve collisions, Physics::getTransforms() here.
        virtual void draw() = 0; // Draw game objects.

        const LayerID getLayerID() const { return m_ID; }

    protected:
        LayerID m_ID = LayerID::UNKNOWN; // Initialize in subclass.

        // Subclasses should create set of game objects, initialize them and manipulate.
        // example:
        // std::vector<std::shared_ptr<Beryll::SceneObject>> m_sceneObjects;    For scene layer.
        // std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;    For GUI layer.
    };
}
