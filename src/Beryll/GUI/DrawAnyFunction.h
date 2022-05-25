#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "GUIObject.h"

namespace Beryll
{
    // class for draw custom function void ...() {...}
    class DrawAnyFunction : public GUIObject
    {
    public:
        DrawAnyFunction(std::function<void()> fn);
        ~DrawAnyFunction() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

    private:
        std::function<void()> m_action;
    };
}
