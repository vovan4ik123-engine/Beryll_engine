#include "AndroidGLESParticleSystem.h"
#include "Beryll/Renderer/Renderer.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Core/TimeStep.h"
#include "Beryll/Core/RandomGenerator.h"
#include "Beryll/Async/AsyncRun.h"
#include "Beryll/Utils/CommonUtils.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESParticleSystem::AndroidGLESParticleSystem()
    {
        m_quadParticles.resize(100000);
        m_currentQuadParticlesIndex = static_cast<int>(m_quadParticles.size()) - 1;

        std::vector<glm::vec3> quadVertices{glm::vec3(-1.0f, -1.0f, 0.0f),
                                            glm::vec3(1.0f, -1.0f, 0.0f),
                                            glm::vec3(1.0f, 1.0f, 0.0f),
                                            glm::vec3(-1.0f, 1.0f, 0.0f)};

        std::vector<uint32_t> quadIndices{0, 1, 2,
                                          2, 3, 0};

        m_quadVertexPosBuffer = Renderer::createVertexBuffer(quadVertices);
        m_quadIndexBuffer = Renderer::createIndexBuffer(quadIndices);
        m_quadVertexArray = Renderer::createVertexArray();
        m_quadVertexArray->addVertexBuffer(m_quadVertexPosBuffer);
        m_quadVertexArray->setIndexBuffer(m_quadIndexBuffer);

        updateQuadParticles = [](std::vector<Particle>& v, int begin, int end) -> void // -> void = return type
        {
            for(int i = begin; i < end; ++i)
            {
                if(!v[i].isActive)
                    continue;

                if(v[i].lifeTimePassed >= v[i].lifeTimeFull)
                {
                    v[i].isActive = false;
                    continue;
                }

                v[i].lifeTimePassed += TimeStep::getTimeStepSec();
                v[i].pos += v[i].moveDir * (v[i].moveSpeed * TimeStep::getTimeStepSec());
                v[i].rotation += ((2.0f * TimeStep::getTimeStepSec()) * v[i].rotationSpeed) * v[i].rotationSide;
                float lifeInRange_0_1 = v[i].lifeTimePassed / v[i].lifeTimeFull;
                v[i].finalColor = glm::lerp(v[i].colorBegin, v[i].colorEnd, lifeInRange_0_1);
                v[i].finalSize = glm::lerp(v[i].sizeBegin, v[i].sizeEnd, lifeInRange_0_1);
                glm::quat faceDirToCameraDirRotation;
                if(Utils::Common::getAngleInRadians(v[i].faceDir, Camera::getCameraDirectionXYZ()) < glm::half_pi<float>())
                    faceDirToCameraDirRotation = glm::rotation(v[i].faceDir, Camera::getCameraDirectionXYZ());
                else
                    faceDirToCameraDirRotation = glm::rotation(-v[i].faceDir, Camera::getCameraDirectionXYZ());

                v[i].MVPMatrix = Camera::getViewProjection() *
                                 // model matrix
                                 (glm::translate(glm::mat4{1.0f}, v[i].pos) *
                                  (glm::toMat4(faceDirToCameraDirRotation) * glm::rotate(glm::mat4{1.0f}, v[i].rotation, v[i].faceDir)) *
                                  glm::scale(glm::mat4{1.0f}, glm::vec3(v[i].finalSize, v[i].finalSize, 1.0f)));
            }
        };

        m_cubeParticles.resize(70000);
        m_currentCubeParticlesIndex = static_cast<int>(m_cubeParticles.size()) - 1;

        std::vector<glm::vec3> cubeVertices;
        cubeVertices.emplace_back(1.0f, -1.0f, -1.0f); // right side +X
        cubeVertices.emplace_back(1.0f, -1.0f, 1.0f);
        cubeVertices.emplace_back(1.0f, 1.0f, -1.0f);
        cubeVertices.emplace_back(1.0f, 1.0f, 1.0f);

        cubeVertices.emplace_back(-1.0f, -1.0f, 1.0f); // left side -X
        cubeVertices.emplace_back(-1.0f, -1.0f, -1.0f);
        cubeVertices.emplace_back(-1.0f, 1.0f, 1.0f);
        cubeVertices.emplace_back(-1.0f, 1.0f, -1.0f);

        cubeVertices.emplace_back(-1.0f, 1.0f, -1.0f); // top +Y
        cubeVertices.emplace_back(1.0f, 1.0f, -1.0f);
        cubeVertices.emplace_back(-1.0f, 1.0f, 1.0f);
        cubeVertices.emplace_back(1.0f, 1.0f, 1.0f);

        cubeVertices.emplace_back(-1.0f, -1.0f, -1.0f); // bottom -Y
        cubeVertices.emplace_back(1.0f, -1.0f, -1.0f);
        cubeVertices.emplace_back(-1.0f, -1.0f, 1.0f);
        cubeVertices.emplace_back(1.0f, -1.0f, 1.0f);

        cubeVertices.emplace_back(1.0f, -1.0f, 1.0f); // front side +Z
        cubeVertices.emplace_back(-1.0f, -1.0f, 1.0f);
        cubeVertices.emplace_back(1.0f, 1.0f, 1.0f);
        cubeVertices.emplace_back(-1.0f, 1.0f, 1.0f);

        cubeVertices.emplace_back(-1.0f, -1.0f, -1.0f); // back side -Z
        cubeVertices.emplace_back(1.0f, -1.0f, -1.0f);
        cubeVertices.emplace_back(-1.0f, 1.0f, -1.0f);
        cubeVertices.emplace_back(1.0f, 1.0f, -1.0f);


        std::vector<uint32_t> cubeIndices{0,1,2,    1,2,3, // two triangles
                                          4,5,6,    5,6,7,
                                          8,9,10,   9,10,11,
                                          12,13,14, 13,14,15,
                                          16,17,18, 17,18,19,
                                          20,21,22, 21,22,23};

        m_cubeVertexPosBuffer = Renderer::createVertexBuffer(cubeVertices);
        m_cubeIndexBuffer = Renderer::createIndexBuffer(cubeIndices);
        m_cubeVertexArray = Renderer::createVertexArray();
        m_cubeVertexArray->addVertexBuffer(m_cubeVertexPosBuffer);
        m_cubeVertexArray->setIndexBuffer(m_cubeIndexBuffer);

        updateCubeParticles = [](std::vector<Particle>& v, int begin, int end) -> void // -> void = return type
        {
            for(int i = begin; i < end; ++i)
            {
                if(!v[i].isActive)
                    continue;

                if(v[i].lifeTimePassed >= v[i].lifeTimeFull)
                {
                    v[i].isActive = false;
                    continue;
                }

                v[i].lifeTimePassed += TimeStep::getTimeStepSec();
                v[i].pos += v[i].moveDir * (v[i].moveSpeed * TimeStep::getTimeStepSec());
                float lifeInRange_0_1 = v[i].lifeTimePassed / v[i].lifeTimeFull;
                v[i].finalColor = glm::lerp(v[i].colorBegin, v[i].colorEnd, lifeInRange_0_1);
                v[i].finalSize = glm::lerp(v[i].sizeBegin, v[i].sizeEnd, lifeInRange_0_1);

                v[i].MVPMatrix = Camera::getViewProjection() *
                                 // model matrix
                                 (glm::translate(glm::mat4{1.0f}, v[i].pos) *
                                  v[i].rotationMatrix *
                                  glm::scale(glm::mat4{1.0f}, glm::vec3(v[i].finalSize, v[i].finalSize, v[i].finalSize)));
            }
        };

        m_internalShader = Renderer::createShader("shaders/GLES/default/Particle.vert", "shaders/GLES/default/Particle.frag");
    }

    AndroidGLESParticleSystem::~AndroidGLESParticleSystem()
    {

    }

    void AndroidGLESParticleSystem::draw()
    {
        m_activeCount = 0;

        if(m_anyQuadParticleIsActive)
        {
            Beryll::AsyncRun::Run(m_quadParticles, AndroidGLESParticleSystem::updateQuadParticles);

            m_internalShader->bind();
            m_quadVertexArray->bind();

            for(const Particle& particle : m_quadParticles)
            {
                if(!particle.isActive)
                    continue;

                ++m_activeCount;

                m_internalShader->set4Float("color", particle.finalColor);
                m_internalShader->setMatrix4x4Float("MVPMatrix", particle.MVPMatrix);

                m_quadVertexArray->draw();
            }
        }

        if(m_anyCubeParticleIsActive)
        {
            Beryll::AsyncRun::Run(m_cubeParticles, AndroidGLESParticleSystem::updateCubeParticles);

            m_internalShader->bind();
            m_cubeVertexArray->bind();

            for(const Particle& particle : m_cubeParticles)
            {
                if(!particle.isActive)
                    continue;

                ++m_activeCount;

                m_internalShader->set4Float("color", particle.finalColor);
                m_internalShader->setMatrix4x4Float("MVPMatrix", particle.MVPMatrix);

                m_cubeVertexArray->draw();
            }
        }

        if(m_activeCount == 0)
        {
            m_anyQuadParticleIsActive = false;
            m_anyCubeParticleIsActive = false;
        }
    }

    void AndroidGLESParticleSystem::EmitQuadsFromCenter(int particlesCount,
                                                         float lifeTime,
                                                         float sizeBegin,
                                                         float sizeEnd,
                                                         glm::vec4 colorBegin,
                                                         glm::vec4 colorEnd,
                                                         glm::vec3 pos,
                                                         glm::vec3 gravity,
                                                         float speed)
    {
        for(int i = 0; i < particlesCount; ++i)
        {
            m_anyQuadParticleIsActive = true;

            Particle& particle = m_quadParticles[m_currentQuadParticlesIndex];
            --m_currentQuadParticlesIndex;
            if(m_currentQuadParticlesIndex < 0)
            {
                m_currentQuadParticlesIndex = static_cast<int>(m_quadParticles.size()) - 1;
            }

            particle.isActive = true;

            particle.lifeTimeFull = lifeTime;
            particle.lifeTimePassed = 0.0f;

            particle.sizeBegin = sizeBegin + sizeBegin * (RandomGenerator::getFloat() - 0.5f);
            particle.sizeEnd = sizeEnd;

            particle.colorBegin = colorBegin + colorBegin * (RandomGenerator::getFloat() - 0.5f) * 0.2f;
            particle.colorEnd = colorEnd;

            particle.pos = pos;

            particle.moveDir.x = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.x * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir.y = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.y * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir.z = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.z * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir = glm::normalize(particle.moveDir);

            particle.moveSpeed = speed * RandomGenerator::getFloat();
            particle.rotation = RandomGenerator::getFloat() * 3.1416f;
            particle.rotationSpeed = RandomGenerator::getFloat();

            if(RandomGenerator::getFloat() < 0.5f)
                particle.rotationSide = -1.0f;
            else
                particle.rotationSide = 1.0f;
        }
    }

    void AndroidGLESParticleSystem::EmitCubesFromCenter(int particlesCount,
                                                         float lifeTime,
                                                         float sizeBegin,
                                                         float sizeEnd,
                                                         glm::vec4 colorBegin,
                                                         glm::vec4 colorEnd,
                                                         glm::vec3 pos,
                                                         glm::vec3 gravity,
                                                         float speed)
    {
        for(int i = 0; i < particlesCount; ++i)
        {
            m_anyCubeParticleIsActive = true;

            Particle& particle = m_cubeParticles[m_currentCubeParticlesIndex];
            --m_currentCubeParticlesIndex;
            if(m_currentCubeParticlesIndex < 0)
            {
                m_currentCubeParticlesIndex = static_cast<int>(m_cubeParticles.size()) - 1;
            }

            particle.isActive = true;

            particle.lifeTimeFull = lifeTime;
            particle.lifeTimePassed = 0.0f;

            particle.sizeBegin = sizeBegin + sizeBegin * (RandomGenerator::getFloat() - 0.5f);
            particle.sizeEnd = sizeEnd;

            particle.colorBegin = colorBegin + colorBegin * (RandomGenerator::getFloat() - 0.5f) * 0.2f;
            particle.colorEnd = colorEnd;

            particle.pos = pos;

            particle.moveDir.x = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.x * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir.y = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.y * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir.z = (RandomGenerator::getFloat() - 0.5f) * 2.0f + gravity.z * std::max(RandomGenerator::getFloat(), 0.2f);
            particle.moveDir = glm::normalize(particle.moveDir);

            particle.moveSpeed = speed * RandomGenerator::getFloat();

            glm::vec3 rotationAxis{0.0f};
            float rotationAngle = RandomGenerator::getFloat() * 3.1416f;
            while(glm::length(rotationAxis) < 0.1f)
            {
                rotationAxis.x = RandomGenerator::getFloat();
                rotationAxis.y = RandomGenerator::getFloat();
                rotationAxis.z = RandomGenerator::getFloat();
            }
            rotationAxis = glm::normalize(rotationAxis);
            particle.rotationMatrix = glm::rotate(glm::mat4{1.0f}, rotationAngle, rotationAxis);
        }
    }
}
