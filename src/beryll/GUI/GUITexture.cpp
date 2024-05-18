#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "MainImGUI.h"

namespace Beryll
{
    GUITexture::GUITexture(const std::string& texturePath, float l, float t, float w, float h, bool bringToFrontOnFocus)
    {
        BR_ASSERT((texturePath.empty() == false), "%s", "Path to texture can not be empty.");

        leftPos = l;
        topPos = t;
        width = w;
        height = h;

        if(!bringToFrontOnFocus)
        {
            m_noBackgroundNoFrame = m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
            m_noFrame = m_noFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
        }

        m_texture = Renderer::createTexture(texturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);
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
                     ImVec2(width * MainImGUI::getInstance()->getGUIWidth() * 0.96f, height * MainImGUI::getInstance()->getGUIHeight() * 0.96f));

        ImGui::End();
    }
}
