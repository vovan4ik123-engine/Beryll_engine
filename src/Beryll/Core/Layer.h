#pragma once

#include "CppHeaders.h"

#include "GameObject.h"

namespace Beryll
{
    enum class LayerID
    {
        UNKNOWN,
        PLAY_GUI,
        PLAY_SCENE,
        PAUSE_GUI
    };

    class Layer
    {
    public:
        virtual ~Layer() {}

        virtual void updateBeforePhysics() = 0; // handle users input, move objects here
        virtual void updateAfterPhysics() = 0; // update positions after simulation, resolve collisions, Physics::getTransforms() here
        virtual void draw() = 0; // draw game objects
        virtual void playSound() = 0; // play sounds

        const LayerID getLayerID() const { return m_ID; };

    protected:
        LayerID m_ID = LayerID::UNKNOWN; // initialize in subclass

        // subclasses should create set of game objects, initialize them and manipulate
        // example:
        // std::vector<std::shared_ptr<Beryll::SceneObject>> m_sceneObjects; for scene layer
        // std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects; git GUI layer
    };
}