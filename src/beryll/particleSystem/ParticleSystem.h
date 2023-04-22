#pragma once

#include "CppHeaders.h"
#include "LibsHeaders.h"

#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"

namespace Beryll
{

    class ParticleSystem
    {
    public:
        ParticleSystem() = delete;
        ~ParticleSystem() = delete;

        static void draw();

        static void EmitQuadsFromCenter(int particlesCount,
                                    float lifeTime,
                                    float sizeBegin,
                                    float sizeEnd,
                                    glm::vec4 colorBegin,
                                    glm::vec4 colorEnd,
                                    glm::vec3 pos,
                                    glm::vec3 gravity,
                                    float speed);

        static void EmitCubesFromCenter(int particlesCount,
                                    float lifeTime,
                                    float sizeBegin,
                                    float sizeEnd,
                                    glm::vec4 colorBegin,
                                    glm::vec4 colorEnd,
                                    glm::vec3 pos,
                                    glm::vec3 gravity,
                                    float speed);

        static int getActiveCount() { return m_activeCount; };

    private:
        friend class GameLoop;
        static void create();

        static int m_activeCount;
        static std::shared_ptr<Shader> m_internalShader;

        struct QuadParticle
        {
            bool isActive = false;

            float lifeTimeFull = 1.0f;
            float lifeTimePassed = 0.0f;

            float sizeBegin = 1.0f;
            float sizeEnd = 0.0f;
            float finalSize = 0.0f;

            glm::vec4 colorBegin{1.0f};
            glm::vec4 colorEnd{0.0f};
            glm::vec4 finalColor{0.0f};

            glm::vec3 pos{0.0f};
            glm::vec3 moveDir{1.0f}; // must be normalized !!! and impacted by gravity (as parameter: glm::vec3 gravity)
            float moveSpeed = 1.0f; // meters in sec

            float rotation = 0.0f;
            float rotationSpeed = 0.0f;
            float rotationSide = 0.0f; // rotate to left or to right. set to -1.0f or +1.0f

            const glm::vec3 faceDir{0.0f, 0.0f, 1.0f};

            glm::mat4 MVPMatrix{1.0f};
            glm::vec4 finalVertex0{0.0f}; // quad original vertices multiplied by MVPMatrix
            glm::vec4 finalVertex1{0.0f};
            glm::vec4 finalVertex2{0.0f};
            glm::vec4 finalVertex3{0.0f};
        };
        static const std::vector<glm::vec4> m_quadVertices;
        static const uint32_t m_maxQuadCount;
        static const uint32_t m_quadVerticesCount; // vertices per quad
        static const uint32_t m_quadIndicesCount; // indices per quad
        static const uint32_t m_allQuadsVerticesCount;
        static const uint32_t m_allQuadsIndicesCount;
        static int m_activeQuads;
        static std::vector<QuadParticle> m_quadParticles;
        static int m_currentQuadParticlesIndex;
        static bool m_anyQuadParticleIsActive; // use to avoid update and draw if no active particles
        static std::shared_ptr<VertexBuffer> m_quadVertexPosDynamicBuffer;
        static std::shared_ptr<VertexBuffer> m_quadVertexColorDynamicBuffer;
        static std::shared_ptr<IndexBuffer> m_quadIndexBuffer;
        static std::unique_ptr<VertexArray> m_quadVertexArray;
        static std::vector<glm::vec4> m_quadVertexPosDynamicVector;
        static std::vector<glm::vec4> m_quadVertexColorDynamicVector;
        static std::function<void(std::vector<QuadParticle>&, int, int)> updateQuadParticles;

        struct CubeParticle
        {
            bool isActive = false;

            float lifeTimeFull = 1.0f;
            float lifeTimePassed = 0.0f;

            float sizeBegin = 1.0f;
            float sizeEnd = 0.0f;
            float finalSize = 0.0f;

            glm::vec4 colorBegin{1.0f};
            glm::vec4 colorEnd{0.0f};
            glm::vec4 finalColor{0.0f};

            glm::vec3 pos{0.0f};
            glm::vec3 moveDir{1.0f}; // must be normalized !!! and impacted by gravity (as parameter: glm::vec3 gravity)
            float moveSpeed = 1.0f; // meters in sec

            glm::mat4 rotationMatrix{1.0f};
            glm::mat4 MVPMatrix{1.0f};
            glm::vec4 finalVertex0{0.0f}; // cube original vertices multiplied by MVPMatrix
            glm::vec4 finalVertex1{0.0f};
            glm::vec4 finalVertex2{0.0f};
            glm::vec4 finalVertex3{0.0f};

            glm::vec4 finalVertex4{0.0f};
            glm::vec4 finalVertex5{0.0f};
            glm::vec4 finalVertex6{0.0f};
            glm::vec4 finalVertex7{0.0f};
        };
        static const std::vector<glm::vec4> m_cubeVertices;
        static const uint32_t m_maxCubeCount;
        static const uint32_t m_cubeVerticesCount; // vertices per cube
        static const uint32_t m_cubeIndicesCount;  // indices per quad
        static const uint32_t m_allCubesVerticesCount;
        static const uint32_t m_allCubesIndicesCount;
        static int m_activeCubes;
        static std::vector<CubeParticle> m_cubeParticles;
        static int m_currentCubeParticlesIndex;
        static bool m_anyCubeParticleIsActive; // use to avoid update and draw if no active particles
        static std::shared_ptr<VertexBuffer> m_cubeVertexPosDynamicBuffer;
        static std::shared_ptr<VertexBuffer> m_cubeVertexColorDynamicBuffer;
        static std::shared_ptr<IndexBuffer> m_cubeIndexBuffer;
        static std::unique_ptr<VertexArray> m_cubeVertexArray;
        static std::vector<glm::vec4> m_cubeVertexPosDynamicVector;
        static std::vector<glm::vec4> m_cubeVertexColorDynamicVector;
        static std::function<void(std::vector<CubeParticle>&, int, int)> updateCubeParticles;
    };
}
