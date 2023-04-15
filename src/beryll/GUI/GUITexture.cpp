#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "MainImGUI.h"

namespace Beryll
{
    GUITexture::GUITexture(const char* texturePath, float left, float top, float width, float height)
            : m_leftPos(left / 100.0f), m_topPos(top / 100.0f), m_width(width / 100.0f), m_height(height / 100.0f)
    {
        BR_ASSERT((std::string(texturePath).empty() == false), "%s", "Path to texture can not be empty");
        BR_ASSERT((left >= 0.0f && left <= 100.0f) && (top >= 0.0f && top <= 100.0f)
                  &&(width >= 0.0f && width <= 100.0f) && (height >= 0.0f && height <= 100.0f), "%s", "Wrong button size or position");

        m_texture = Renderer::createTexture(texturePath, TextureType::DIFFUSE_TEXTURE);
    }

    GUITexture::~GUITexture()
    {

    }

    void GUITexture::updateBeforePhysics()
    {

    }

    void GUITexture::updateAfterPhysics()
    {

    }

    void GUITexture::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_stringID.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::Image(reinterpret_cast<ImTextureID>(m_texture->getID()),
                     ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();
    }
}
