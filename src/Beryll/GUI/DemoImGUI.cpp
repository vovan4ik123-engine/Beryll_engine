#include "DemoImGUI.h"

namespace Beryll
{
    DemoImGUI::DemoImGUI()
    {

    }

    DemoImGUI::~DemoImGUI()
    {

    }

    void DemoImGUI::updateBeforePhysics()
    {

    }

    void DemoImGUI::updateAfterPhysics()
    {

    }

    void DemoImGUI::draw()
    {
        bool showDemo = true;
        ImGui::ShowDemoWindow(&showDemo);
    }

    void DemoImGUI::playSound()
    {

    }
}
