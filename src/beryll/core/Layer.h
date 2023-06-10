#pragma once

#include "CppHeaders.h"

#include "GameObject.h"

namespace Beryll
{
    enum class LayerID
    {
        UNKNOWN,
        START_SCREEN_GUI,
        PLAY_GUI,
        PLAY_SCENE,
        PAUSE_GUI,
        SETTINGS_GUI
    };

    class Layer
    {
    public:
        virtual ~Layer() {}

        virtual void updateBeforePhysics() = 0; // Handle users input, move objects here.
        virtual void updateAfterPhysics() = 0; // Update positions after simulation, resolve collisions, Physics::getTransforms() here.
        virtual void draw() = 0; // Draw game objects.

        const LayerID getLayerID() const { return m_ID; };

    protected:
        LayerID m_ID = LayerID::UNKNOWN; // Initialize in subclass.

        // Subclasses should create set of game objects, initialize them and manipulate.
        // example:
        // std::vector<std::shared_ptr<Beryll::SceneObject>> m_sceneObjects; for scene layer
        // std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects; git GUI layer
    };
}
