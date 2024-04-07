#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "MainImGUI.h"

namespace Beryll
{
    GUITexture::GUITexture(const std::string& texturePath, float l, float t, float w, float h)
    {
        BR_ASSERT((texturePath.empty() == false), "%s", "Path to texture can not be empty.");

        leftPos = l / 100.0f;
        topPos = t / 100.0f;
        width = w / 100.0f;
        height = h / 100.0f;

        m_texture = Renderer::createTexture(texturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_noBackgroundNoFrame = m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
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
        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::Image(reinterpret_cast<ImTextureID>(m_texture->getID()),
                     ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();
    }
}
