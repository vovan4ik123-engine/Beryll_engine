#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class CheckBox : public GUIObject
    {
    public:
        CheckBox() = delete;
        /*
         * unMarkedTexturePath - Cannot be empty.
         * markedTexturePath - Cannot be empty.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         */
        CheckBox(const char* unMarkedTexturePath,
                 const char* markedTexturePath,
                 const glm::vec3& pos, const glm::vec2& widthHeight);
        ~CheckBox() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool marked = false;
        bool getIsMarking() { return m_marking; }
        bool getIsUnMarking() { return m_unMarking; }

        void setAction(std::function<void()> fn) { m_action = fn; }

    private:
        // If action is set m_action() will called once when marked.
        std::function<void()> m_action;

        bool m_marking = false; // Value changing to marked.
        bool m_unMarking = false; // Value changing to un marked.

        // Vertex and index buffers are in base class.
        // ........
        std::unique_ptr<Texture> m_unMarkedTexture;
        std::unique_ptr<Texture> m_markedTexture;
    };
}
