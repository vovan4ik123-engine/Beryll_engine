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

        std::vector<Particle> m_quadParticles;
        int m_currentQuadParticlesIndex = 0;
        bool m_anyQuadParticleIsActive = false; // use to avoid update and draw if no active particles
        std::shared_ptr<VertexBuffer> m_quadVertexPosBuffer;
        std::shared_ptr<IndexBuffer> m_quadIndexBuffer;
        std::unique_ptr<VertexArray> m_quadVertexArray;
        std::function<void(std::vector<Particle>&, int, int)> updateQuadParticles;

        std::vector<Particle> m_cubeParticles;
        int m_currentCubeParticlesIndex = 0;
        bool m_anyCubeParticleIsActive = false; // use to avoid update and draw if no active particles
        std::shared_ptr<VertexBuffer> m_cubeVertexPosBuffer;
        std::shared_ptr<IndexBuffer> m_cubeIndexBuffer;
        std::unique_ptr<VertexArray> m_cubeVertexArray;
        std::function<void(std::vector<Particle>&, int, int)> updateCubeParticles;
    };
}
