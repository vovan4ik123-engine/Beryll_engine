#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"
#include "beryll/renderer/SkyBox.h"

namespace Beryll
{
    class AndroidGLESSkyBox : public SkyBox
    {
        AndroidGLESSkyBox() = delete;
        ~AndroidGLESSkyBox() override;

        void draw() override;

    private:
        friend class Renderer;
        /*
         * folderPath - path to folder in asset folder which contains 6 .jpg textures for every cube side
         */
        AndroidGLESSkyBox(std::string folderPath);

        uint32_t m_openGLID;

        std::shared_ptr<Shader> m_internalShader;

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;

        glm::mat4 m_persp{1.0f};
        glm::mat4 m_view{1.0f};
        glm::mat4 m_perspView{1.0f};
    };

}