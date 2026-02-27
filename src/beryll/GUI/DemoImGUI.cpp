#include "DemoImGUI.h"

namespace Beryll
{
    DemoImGUI::DemoImGUI()  : GUIObject(glm::vec3{0.0f}, glm::vec2{0.0f})
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
}
