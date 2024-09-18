#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    struct NumberToShow
    {
        NumberToShow(int pNumber, float pHeight, float pLifetime, glm::vec3 pOrigin, glm::vec3 pMoveDir, float pMoveSpeed)
        : number(pNumber), numberAsString(std::to_string(pNumber)), height(pHeight), lifetimeSec(pLifetime),
          origin(pOrigin), moveDir(pMoveDir), moveSpeed(pMoveSpeed) {}

        int number = 0;
        std::string numberAsString;
        float height = 1.0f;
        float lifetimeSec = 0.0f;
        glm::vec3 origin{0.0f};
        glm::vec3 moveDir{0.0f};
        float moveSpeed = 0.0f;
    };

    class TextOnScene
    {
    public:
        TextOnScene() = delete;
        ~TextOnScene() = delete;

        static void addNumbersToShow(int pNumber, float pHeight, float pLifetime, glm::vec3 pOrigin, glm::vec3 pMoveDir, float pMoveSpeed);
        static void draw();
        static void setMaxCountToShow(int count) { m_maxCountToShow = count; }

    private:
        friend class GameLoop;
        static void create();
        static bool m_created;

        static std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        static std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        static std::shared_ptr<IndexBuffer> m_indexBuffer;
        static std::unique_ptr<VertexArray> m_vertexArray;
        static std::shared_ptr<Shader> m_internalShader;
        static std::vector<std::unique_ptr<Texture>> m_textures;

        static std::vector<NumberToShow> m_numbersToShow;
        static int m_maxCountToShow;
    };
}
