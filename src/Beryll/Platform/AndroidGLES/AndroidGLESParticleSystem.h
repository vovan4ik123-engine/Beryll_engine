#pragma once

#include "Beryll/ParticleSystem/ParticleSystem.h"
#include "Beryll/Renderer/Shader.h"
#include "Beryll/Renderer/Buffer.h"
#include "Beryll/Renderer/VertexArray.h"

namespace Beryll
{
    class AndroidGLESParticleSystem : public ParticleSystem
    {
        ~AndroidGLESParticleSystem() override;

        void draw() override;

        void EmitQuadsFromCenter(int particlesCount,
                                 float lifeTime,
                                 float sizeBegin,
                                 float sizeEnd,
                                 glm::vec4 colorBegin,
                                 glm::vec4 colorEnd,
                                 glm::vec3 pos,
                                 glm::vec3 gravity,
                                 float speed) override;

        void EmitCubesFromCenter(int particlesCount,
                                 float lifeTime,
                                 float sizeBegin,
                                 float sizeEnd,
                                 glm::vec4 colorBegin,
                                 glm::vec4 colorEnd,
                                 glm::vec3 pos,
                                 glm::vec3 gravity,
                                 float speed) override;

        int getActiveCount() override { return m_activeCount; };

    private:
        friend class ParticleSystem;
        AndroidGLESParticleSystem();

        int m_activeCount = 0;
        std::shared_ptr<Shader> m_internalShader;

        const uint32_t m_maxQuadCount = 100000;
        const uint32_t m_quadVerticesCount = 4; // 4 vertices per quad
        const uint32_t m_quadIndicesCount = 6;  // 6 indices per quad
        const uint32_t m_allQuadsVerticesCount = m_maxQuadCount * m_quadVerticesCount;
        const uint32_t m_allQuadsIndicesCount = m_maxQuadCount * m_quadIndicesCount;
        int m_activeQuads = 0;
        std::vector<QuadParticle> m_quadParticles;
        int m_currentQuadParticlesIndex = 0;
        bool m_anyQuadParticleIsActive = false; // use to avoid update and draw if no active particles
        std::shared_ptr<VertexBuffer> m_quadVertexPosDynamicBuffer;
        std::shared_ptr<VertexBuffer> m_quadVertexColorDynamicBuffer;
        std::shared_ptr<IndexBuffer> m_quadIndexBuffer;
        std::unique_ptr<VertexArray> m_quadVertexArray;
        std::vector<glm::vec4> m_quadsVertDynamicVector;
        std::vector<glm::vec4> m_quadsColDynamicVector;
        std::function<void(std::vector<QuadParticle>&, int, int)> updateQuadParticles;

        const uint32_t m_maxCubeCount = 100000;
        const uint32_t m_cubeVerticesCount = 8; // 8 vertices per cube
        const uint32_t m_cubeIndicesCount = 36;  // 36 indices per quad
        const uint32_t m_allCubesVerticesCount = m_maxCubeCount * m_cubeVerticesCount;
        const uint32_t m_allCubesIndicesCount = m_maxCubeCount * m_cubeIndicesCount;
        int m_activeCubes = 0;
        std::vector<CubeParticle> m_cubeParticles;
        int m_currentCubeParticlesIndex = 0;
        bool m_anyCubeParticleIsActive = false; // use to avoid update and draw if no active particles
        std::shared_ptr<VertexBuffer> m_cubeVertexPosDynamicBuffer;
        std::shared_ptr<VertexBuffer> m_cubeVertexColorDynamicBuffer;
        std::shared_ptr<IndexBuffer> m_cubeIndexBuffer;
        std::unique_ptr<VertexArray> m_cubeVertexArray;
        std::vector<glm::vec4> m_cubesVertDynamicVector;
        std::vector<glm::vec4> m_cubesColDynamicVector;
        std::function<void(std::vector<CubeParticle>&, int, int)> updateCubeParticles;
    };
}
