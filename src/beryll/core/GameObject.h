#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/physics/Physics.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    // base class of all game objects in game (3d scene and GUI objects)
    // game objects are handled in layers in game state
    class GameObject
    {
    public:
        virtual ~GameObject() { }

        virtual void updateBeforePhysics() = 0; // handle users input, move objects here
        virtual void updateAfterPhysics() = 0; // update positions after simulation, resolve collisions, Physics::getTransforms() here
        virtual void draw() = 0; // subclass graphics

        int getID() { return m_ID; }

    protected:
        const int m_ID = Utils::Common::generateID(); // unique
    };
}
