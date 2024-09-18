#include "TextOnScene.h"
#include "beryll/core/BeryllConstants.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    bool TextOnScene::m_created = false;
    std::shared_ptr<VertexBuffer> TextOnScene::m_vertexPosBuffer;
    std::shared_ptr<VertexBuffer> TextOnScene::m_textureCoordsBuffer;
    std::shared_ptr<IndexBuffer> TextOnScene::m_indexBuffer;
    std::unique_ptr<VertexArray> TextOnScene::m_vertexArray;
    std::shared_ptr<Shader> TextOnScene::m_internalShader;
    std::vector<std::unique_ptr<Texture>> TextOnScene::m_textures;

    std::vector<NumberToShow> TextOnScene::m_numbersToShow;
    int TextOnScene::m_maxCountToShow = 7;

    void TextOnScene::create()
    {
        if(m_created) { return; }

        std::vector<glm::vec3> vertices{glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3( 0.0f, 0.0f, 1.0f),
                                        glm::vec3( 0.0f,  1.0f, 1.0f),
                                        glm::vec3(0.0f,  1.0f, 0.0f)};

#if defined(ANDROID)
        std::vector<glm::vec2> textureCoords{glm::vec2(0.0f, 1.0f), // Flipped Y for OpenGL.
                                             glm::vec2(1.0f, 1.0f),
                                             glm::vec2(1.0f, 0.0f),
                                             glm::vec2(0.0f, 0.0f)};
#elif defined(APPLE)

#endif

        std::vector<uint32_t> indices{0,1,2,
                                      2,3,0};

        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::textOnSceneVertexPath.data(),
                                                  BeryllConstants::textOnSceneFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();

        std::string path;
        for(int i = 0; i < 10; ++i)
        {
            path = "fonts/numbers/";
            path += std::to_string(i);
            path += ".png";
            m_textures.emplace_back(Beryll::Renderer::createTexture(path.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1));
        }

        BR_INFO("m_textures.size() %d", m_textures.size());

        m_numbersToShow.reserve(100);

        m_created = true;
    }

    void TextOnScene::addNumbersToShow(int pNumber, float pHeight, float pLifetime, glm::vec3 pOrigin, glm::vec3 pMoveDir, float pMoveSpeed)
    {
        if(m_numbersToShow.size() >= m_maxCountToShow)
            return;

        m_numbersToShow.emplace_back(pNumber, pHeight, pLifetime, pOrigin, glm::normalize(pMoveDir), pMoveSpeed);

        std::sort(m_numbersToShow.begin(), m_numbersToShow.end(), [](const NumberToShow& n1, const NumberToShow& n2)
                  {
                      return glm::distance(Beryll::Camera::getCameraPos(), n1.origin) > glm::distance(Beryll::Camera::getCameraPos(), n2.origin);
                  });
    }

    void TextOnScene::draw()
    {
        if(m_numbersToShow.empty())
            return;

        m_numbersToShow.erase(std::remove_if(m_numbersToShow.begin(), m_numbersToShow.end(),
                                             [](const NumberToShow& x) { return x.lifetimeSec <= 0.0f; }),
                              m_numbersToShow.end());

        // Prepare and calculate data.
        for(NumberToShow& toShow : m_numbersToShow)
        {
            float totalWidth = 0;
            for(const char ch : toShow.numberAsString)
            {
                int ASCII_charToInt = int(ch) - int('0');
                // height * aspect ratio(width / height).
                totalWidth += toShow.height * static_cast<float>(m_textures[ASCII_charToInt]->getWidth()) / static_cast<float>(m_textures[ASCII_charToInt]->getHeight());
            }
            const float halfOfTotalWidth = totalWidth * 0.5f;

            toShow.lifetimeSec -= Beryll::TimeStep::getTimeStepSec();
            if(toShow.moveSpeed > 0.0f)
                toShow.origin += toShow.moveDir * (toShow.moveSpeed * Beryll::TimeStep::getTimeStepSec());

            float textureAspectRation = 0;
            float numberWidth = 0;
            float previousNumbersWidth = 0;
            glm::mat4 scale{1.0f};
            glm::mat4 translate{1.0f};
            glm::mat4 rot = glm::toMat4(glm::rotation(glm::vec3(0.0f, 1.0f, 0.0f), Beryll::Camera::getCameraUp()) *
                                        glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), Beryll::Camera::getCameraFrontDirectionXZ()));
            for(const char ch : toShow.numberAsString)
            {
                int ASCII_charToInt = int(ch) - int('0');
                textureAspectRation = static_cast<float>(m_textures[ASCII_charToInt]->getWidth()) / static_cast<float>(m_textures[ASCII_charToInt]->getHeight());
                numberWidth = toShow.height * textureAspectRation;
                scale = glm::scale(glm::mat4{1.0f}, glm::vec3(0.0f, toShow.height, numberWidth));
                translate = glm::translate(glm::mat4{1.0f}, toShow.origin + Beryll::Camera::getCameraRightXZ() * (previousNumbersWidth - halfOfTotalWidth));
                previousNumbersWidth += numberWidth;

                m_internalShader->bind();
                m_internalShader->setMatrix4x4Float("MVPMatrix", Beryll::Camera::getViewProjection() * (translate * rot * scale));
                m_textures[ASCII_charToInt]->bind();
                m_vertexArray->bind();
                m_vertexArray->draw();
            }
        }
    }
}
