#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class DemoImGUI : public GUIObject
    {
    public:
        DemoImGUI();
        ~DemoImGUI() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
    };
}
