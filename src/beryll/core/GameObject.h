#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/utils/CommonUtils.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    // Base class of all game objects in game (3d scene and GUI objects).
    // Game objects are handled in layers in game state.
    class GameObject
    {
    public:
        virtual ~GameObject() { }

        virtual void updateBeforePhysics() = 0; // Handle users input, move objects here.
        virtual void updateAfterPhysics() = 0; // Update positions after simulation, resolve collisions, Physics::getTransforms() here.
        virtual void draw() = 0; // Subclass graphics.

        const int getID() const { return m_ID; }

    protected:
        const int m_ID = BeryllUtils::Common::generateID(); // Unique.
    };
}
