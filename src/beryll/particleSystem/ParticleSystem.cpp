#include "ParticleSystem.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/RandomGenerator.h"
#include "beryll/async/AsyncRun.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/GUI/MainImGUI.h"

namespace Beryll
{
    int ParticleSystem::m_activeCount = 0;
    std::shared_ptr<Shader> ParticleSystem::m_internalShader;
    Timer ParticleSystem::m_timer;
    float ParticleSystem::m_timeStep = 0.0f;

    const std::vector<glm::vec4> ParticleSystem::m_quadVertices{glm::vec4{-1.0f, -1.0f, 0.0f, 1.0f},
                                                                glm::vec4{1.0f, -1.0f, 0.0f, 1.0f},
                                                                glm::vec4{1.0f, 1.0f, 0.0f, 1.0f},
                                                                glm::vec4{-1.0f, 1.0f, 0.0f, 1.0f}};
    const uint32_t ParticleSystem::m_maxQuadCount = 100000;
    const uint32_t ParticleSystem::m_quadVerticesCount = 4; // 4 vertices per quad.
    const uint32_t ParticleSystem::m_quadIndicesCount = 6;  // 6 indices per quad.
    const uint32_t ParticleSystem::m_allQuadsVerticesCount = m_maxQuadCount * m_quadVerticesCount;
    const uint32_t ParticleSystem::m_allQuadsIndicesCount = m_maxQuadCount * m_quadIndicesCount;
    int ParticleSystem::m_activeQuads = 0;
    std::vector<ParticleSystem::QuadParticle> ParticleSystem::m_quadParticles;
    int ParticleSystem::m_currentQuadParticlesIndex = 0;
    bool ParticleSystem::m_anyQuadParticleIsActive = false; // Use to avoid update and draw if no active particles.
    std::shared_ptr<VertexBuffer> ParticleSystem::m_quadVertexPosDynamicBuffer;
    std::shared_ptr<VertexBuffer> ParticleSystem::m_quadVertexColorDynamicBuffer;
    std::shared_ptr<IndexBuffer> ParticleSystem::m_quadIndexBuffer;
    std::unique_ptr<VertexArray> ParticleSystem::m_quadVertexArray;
    std::vector<glm::vec4> ParticleSystem::m_quadVertexPosDynamicVector;
    std::vector<glm::vec4> ParticleSystem::m_quadVertexColorDynamicVector;
    std::function<void(std::vector<ParticleSystem::QuadParticle>&, int, int)> ParticleSystem::updateQuadParticles;

    const std::vector<glm::vec4> ParticleSystem::m_cubeVertices{glm::vec4{-1.0f, -1.0f, 1.0f, 1.0f}, // Front side +Z.
                                                                glm::vec4{1.0f, -1.0f, 1.0f, 1.0f},
                                                                glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
                                                                glm::vec4{-1.0f, 1.0f, 1.0f, 1.0f},

                                                                glm::vec4{-1.0f, -1.0f, -1.0f, 1.0f}, // Back side -Z.
                                                                glm::vec4{1.0f, -1.0f, -1.0f, 1.0f},
                                                                glm::vec4{1.0f, 1.0f, -1.0f, 1.0f},
                                                                glm::vec4{-1.0f, 1.0f, -1.0f, 1.0f}};
    const uint32_t ParticleSystem::m_maxCubeCount = 100000;
    const uint32_t ParticleSystem::m_cubeVerticesCount = 8; // 8 vertices per cube.
    const uint32_t ParticleSystem::m_cubeIndicesCount = 36;  // 36 indices per quad.
    const uint32_t ParticleSystem::m_allCubesVerticesCount = m_maxCubeCount * m_cubeVerticesCount;
    const uint32_t ParticleSystem::m_allCubesIndicesCount = m_maxCubeCount * m_cubeIndicesCount;
    int ParticleSystem::m_activeCubes = 0;
    std::vector<ParticleSystem::CubeParticle> ParticleSystem::m_cubeParticles;
    int ParticleSystem::m_currentCubeParticlesIndex = 0;
    bool ParticleSystem::m_anyCubeParticleIsActive = false; // Use to avoid update and draw if no active particles.
    std::shared_ptr<VertexBuffer> ParticleSystem::m_cubeVertexPosDynamicBuffer;
    std::shared_ptr<VertexBuffer> ParticleSystem::m_cubeVertexColorDynamicBuffer;
    std::shared_ptr<IndexBuffer> ParticleSystem::m_cubeIndexBuffer;
    std::unique_ptr<VertexArray> ParticleSystem::m_cubeVertexArray;
    std::vector<glm::vec4> ParticleSystem::m_cubeVertexPosDynamicVector;
    std::vector<glm::vec4> ParticleSystem::m_cubeVertexColorDynamicVector;
    std::function<void(std::vector<ParticleSystem::CubeParticle>&, int, int)> ParticleSystem::updateCubeParticles;

    void ParticleSystem::create()
    {
        if(!m_quadParticles.empty() && !m_cubeParticles.empty()) { return; }

        m_quadParticles.resize(m_maxQuadCount);
        m_currentQuadParticlesIndex = static_cast<int>(m_quadParticles.size()) - 1;

        std::vector<uint32_t> quadsIndices;
        quadsIndices.resize(m_allQuadsIndicesCount);
        uint32_t quadsIndicesOffset = 0;
        for(uint32_t i = 0; i < m_allQuadsIndicesCount; i += m_quadIndicesCount)
        {
            quadsIndices[i + 0] = 0 + quadsIndicesOffset;
            quadsIndices[i + 1] = 1 + quadsIndicesOffset;
            quadsIndices[i + 2] = 2 + quadsIndicesOffset;

            quadsIndices[i + 3] = 2 + quadsIndicesOffset;
            quadsIndices[i + 4] = 3 + quadsIndicesOffset;
            quadsIndices[i + 5] = 0 + quadsIndicesOffset;

            quadsIndicesOffset += m_quadVerticesCount; // Deference between current and next quad indices.
        }

        uint32_t allQuadsVerticesSizeBytes = m_allQuadsVerticesCount * sizeof(glm::vec4); // 1 vertex = 4 floats.
        m_quadVertexPosDynamicVector.resize(m_allQuadsVerticesCount);
        m_quadVertexColorDynamicVector.resize(m_allQuadsVerticesCount);

        m_quadVertexPosDynamicBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::FOUR, allQuadsVerticesSizeBytes);
        m_quadVertexColorDynamicBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::FOUR, allQuadsVerticesSizeBytes);
        m_quadIndexBuffer = Renderer::createStaticIndexBuffer(quadsIndices);

        m_quadVertexArray = Renderer::createVertexArray();
        m_quadVertexArray->addVertexBuffer(m_quadVertexPosDynamicBuffer);
        m_quadVertexArray->addVertexBuffer(m_quadVertexColorDynamicBuffer);
        m_quadVertexArray->setIndexBuffer(m_quadIndexBuffer);

        updateQuadParticles = [](std::vector<QuadParticle>& v, int begin, int end) -> void // -> void = return type.
        {
            for(int i = begin; i < end; ++i)
            {
                if(!v[i].isActive)
                    continue;

                v[i].lifeTimePassed += m_timeStep;

                if(v[i].lifeTimePassed >= v[i].lifeTimeFull)
                {
                    v[i].isActive = false;
                    continue;
                }

                v[i].pos += v[i].moveDir * (v[i].moveSpeed * m_timeStep);
                v[i].rotation += ((2.0f * m_timeStep) * v[i].rotationSpeed) * v[i].rotationSide;
                float lifeInRange_0_1 = v[i].lifeTimePassed / v[i].lifeTimeFull;
                v[i].finalColor = glm::lerp(v[i].colorBegin, v[i].colorEnd, lifeInRange_0_1);
                v[i].finalSize = glm::lerp(v[i].sizeBegin, v[i].sizeEnd, lifeInRange_0_1);
                glm::quat faceDirToCameraDirRotation;
                if(Utils::Common::getAngleInRadians(v[i].faceDir, Camera::getCameraDirectionXYZ()) < glm::half_pi<float>())
                    faceDirToCameraDirRotation = glm::rotation(v[i].faceDir, Camera::getCameraDirectionXYZ());
                else
                    faceDirToCameraDirRotation = glm::rotation(-v[i].faceDir, Camera::getCameraDirectionXYZ());

                v[i].MVPMatrix = Camera::getViewProjection() *
                                 // Model matrix.
                                 (glm::translate(glm::mat4{1.0f}, v[i].pos) *
                                  (glm::toMat4(faceDirToCameraDirRotation) * glm::rotate(glm::mat4{1.0f}, v[i].rotation, v[i].faceDir)) *
                                  glm::scale(glm::mat4{1.0f}, glm::vec3{v[i].finalSize, v[i].finalSize, 1.0f}));

                v[i].finalVertex0 = v[i].MVPMatrix * ParticleSystem::m_quadVertices[0];
                v[i].finalVertex1 = v[i].MVPMatrix * ParticleSystem::m_quadVertices[1];
                v[i].finalVertex2 = v[i].MVPMatrix * ParticleSystem::m_quadVertices[2];
                v[i].finalVertex3 = v[i].MVPMatrix * ParticleSystem::m_quadVertices[3];
            }
        };

        m_cubeParticles.resize(m_maxCubeCount);
        m_currentCubeParticlesIndex = static_cast<int>(m_cubeParticles.size()) - 1;

        std::vector<uint32_t> cubesIndices;
        cubesIndices.resize(m_allCubesIndicesCount);
        uint32_t cubesIndicesOffset = 0;
        for(uint32_t i = 0; i < m_allCubesIndicesCount; i += m_cubeIndicesCount)
        {
            cubesIndices[i + 0]  = 0 + cubesIndicesOffset;
            cubesIndices[i + 1]  = 1 + cubesIndicesOffset;
            cubesIndices[i + 2]  = 2 + cubesIndicesOffset;
            cubesIndices[i + 3]  = 2 + cubesIndicesOffset;
            cubesIndices[i + 4]  = 3 + cubesIndicesOffset;
            cubesIndices[i + 5]  = 0 + cubesIndicesOffset;

            cubesIndices[i + 6]  = 1 + cubesIndicesOffset;
            cubesIndices[i + 7]  = 5 + cubesIndicesOffset;
            cubesIndices[i + 8]  = 6 + cubesIndicesOffset;
            cubesIndices[i + 9]  = 6 + cubesIndicesOffset;
            cubesIndices[i + 10] = 2 + cubesIndicesOffset;
            cubesIndices[i + 11] = 1 + cubesIndicesOffset;

            cubesIndices[i + 12] = 5 + cubesIndicesOffset;
            cubesIndices[i + 13] = 4 + cubesIndicesOffset;
            cubesIndices[i + 14] = 7 + cubesIndicesOffset;
            cubesIndices[i + 15] = 7 + cubesIndicesOffset;
            cubesIndices[i + 16] = 6 + cubesIndicesOffset;
            cubesIndices[i + 17] = 5 + cubesIndicesOffset;

            cubesIndices[i + 18] = 4 + cubesIndicesOffset;
            cubesIndices[i + 19] = 0 + cubesIndicesOffset;
            cubesIndices[i + 20] = 3 + cubesIndicesOffset;
            cubesIndices[i + 21] = 3 + cubesIndicesOffset;
            cubesIndices[i + 22] = 7 + cubesIndicesOffset;
            cubesIndices[i + 23] = 4 + cubesIndicesOffset;

            cubesIndices[i + 24] = 3 + cubesIndicesOffset;
            cubesIndices[i + 25] = 2 + cubesIndicesOffset;
            cubesIndices[i + 26] = 6 + cubesIndicesOffset;
            cubesIndices[i + 27] = 6 + cubesIndicesOffset;
            cubesIndices[i + 28] = 7 + cubesIndicesOffset;
            cubesIndices[i + 29] = 3 + cubesIndicesOffset;

            cubesIndices[i + 30] = 4 + cubesIndicesOffset;
            cubesIndices[i + 31] = 5 + cubesIndicesOffset;
            cubesIndices[i + 32] = 1 + cubesIndicesOffset;
            cubesIndices[i + 33] = 1 + cubesIndicesOffset;
            cubesIndices[i + 34] = 0 + cubesIndicesOffset;
            cubesIndices[i + 35] = 4 + cubesIndicesOffset;

            cubesIndicesOffset += m_cubeVerticesCount; // Deference between current and next cube indices.
        }

        /*
        std::vector<uint32_t> cubeIndices{0,1,2,    2,3,0, // Two triangles.
                                          1,5,6,    6,2,1,
                                          5,4,7,    7,6,5,
                                          4,0,3,    3,7,4,
                                          3,2,6,    6,7,3,
                                          4,5,1,    1,0,4}; */

        uint32_t allCubesVerticesSizeBytes = m_allCubesVerticesCount * sizeof(glm::vec4); // 1 vertex = 4 floats.
        m_cubeVertexPosDynamicVector.resize(m_allCubesVerticesCount);
        m_cubeVertexColorDynamicVector.resize(m_allCubesVerticesCount);

        m_cubeVertexPosDynamicBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::FOUR, allCubesVerticesSizeBytes);
        m_cubeVertexColorDynamicBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::FOUR, allCubesVerticesSizeBytes);
        m_cubeIndexBuffer = Renderer::createStaticIndexBuffer(cubesIndices);

        m_cubeVertexArray = Renderer::createVertexArray();
        m_cubeVertexArray->addVertexBuffer(m_cubeVertexPosDynamicBuffer);
        m_cubeVertexArray->addVertexBuffer(m_cubeVertexColorDynamicBuffer);
        m_cubeVertexArray->setIndexBuffer(m_cubeIndexBuffer);

        updateCubeParticles = [](std::vector<CubeParticle>& v, int begin, int end) -> void // -> void = return type.
        {
            for(int i = begin; i < end; ++i)
            {
                if(!v[i].isActive)
                    continue;

                v[i].lifeTimePassed += m_timeStep;

                if(v[i].lifeTimePassed >= v[i].lifeTimeFull)
                {
                    v[i].isActive = false;
                    continue;
                }

                v[i].pos += v[i].moveDir * (v[i].moveSpeed * m_timeStep);
                float lifeInRange_0_1 = v[i].lifeTimePassed / v[i].lifeTimeFull;
                v[i].finalColor = glm::lerp(v[i].colorBegin, v[i].colorEnd, lifeInRange_0_1);
                v[i].finalSize = glm::lerp(v[i].sizeBegin, v[i].sizeEnd, lifeInRange_0_1);

                v[i].MVPMatrix = Camera::getViewProjection() *
                                 // Model matrix.
                                 (glm::translate(glm::mat4{1.0f}, v[i].pos) *
                                  v[i].rotationMatrix *
                                  glm::scale(glm::mat4{1.0f}, glm::vec3{v[i].finalSize}));

                v[i].finalVertex0 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[0];
                v[i].finalVertex1 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[1];
                v[i].finalVertex2 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[2];
                v[i].finalVertex3 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[3];
                v[i].finalVertex4 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[4];
                v[i].finalVertex5 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[5];
                v[i].finalVertex6 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[6];
                v[i].finalVertex7 = v[i].MVPMatrix * ParticleSystem::m_cubeVertices[7];
            }
        };

        m_internalShader = Renderer::createShader(BeryllConstants::particleVertexPath.data(),
                                                  BeryllConstants::particleFragmentPath.data());

        m_timer.reset();
    }

    void ParticleSystem::draw()
    {
        BR_ASSERT((!m_quadParticles.empty() && !m_cubeParticles.empty()), "%s", "Create Particle system before use");

        m_timeStep = m_timer.getElapsedSec();
        m_timer.reset();

        m_activeCount = 0;

        m_activeQuads = 0;
        if(m_anyQuadParticleIsActive)
        {
            uint32_t vertexOffset = 0;
            Beryll::AsyncRun::Run(m_quadParticles, ParticleSystem::updateQuadParticles);

            for(const QuadParticle& particle : m_quadParticles)
            {
                if(!particle.isActive)
                    continue;

                ++m_activeQuads;

                // Collect dynamic positions and colors.
                m_quadVertexPosDynamicVector[vertexOffset + 0] = particle.finalVertex0;     m_quadVertexColorDynamicVector[vertexOffset + 0] = particle.finalColor;
                m_quadVertexPosDynamicVector[vertexOffset + 1] = particle.finalVertex1;     m_quadVertexColorDynamicVector[vertexOffset + 1] = particle.finalColor;
                m_quadVertexPosDynamicVector[vertexOffset + 2] = particle.finalVertex2;     m_quadVertexColorDynamicVector[vertexOffset + 2] = particle.finalColor;
                m_quadVertexPosDynamicVector[vertexOffset + 3] = particle.finalVertex3;     m_quadVertexColorDynamicVector[vertexOffset + 3] = particle.finalColor;

                vertexOffset += m_quadVerticesCount;
            }

            m_activeCount += m_activeQuads;

            if(m_activeQuads > 0)
            {
                m_quadVertexPosDynamicBuffer->setDynamicBufferData(m_quadVertexPosDynamicVector, m_activeQuads * m_quadVerticesCount);
                m_quadVertexColorDynamicBuffer->setDynamicBufferData(m_quadVertexColorDynamicVector, m_activeQuads * m_quadVerticesCount);
                m_quadIndexBuffer->setCount(m_activeQuads * m_quadIndicesCount);

                m_internalShader->bind();
                m_quadVertexArray->bind();
                m_quadVertexArray->draw();
            }
        }

        if(m_activeQuads == 0)
            m_anyQuadParticleIsActive = false;

        m_activeCubes = 0;
        if(m_anyCubeParticleIsActive)
        {
            uint32_t vertexOffset = 0;
            Beryll::AsyncRun::Run(m_cubeParticles, ParticleSystem::updateCubeParticles);

            for(const CubeParticle& particle : m_cubeParticles)
            {
                if(!particle.isActive)
                    continue;

                ++m_activeCubes;

                //BR_INFO("pos X: %f Y: %f Z: %f", particle.pos.x, particle.pos.y, particle.pos.z);
                //BR_INFO("particle X: %f Y: %f Z: %f", particle.MVPMatrix[3][0], particle.MVPMatrix[3][1], particle.MVPMatrix[3][2]);

                // Collect dynamic positions and colors.
                m_cubeVertexPosDynamicVector[vertexOffset + 0] = particle.finalVertex0;     m_cubeVertexColorDynamicVector[vertexOffset + 0] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 1] = particle.finalVertex1;     m_cubeVertexColorDynamicVector[vertexOffset + 1] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 2] = particle.finalVertex2;     m_cubeVertexColorDynamicVector[vertexOffset + 2] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 3] = particle.finalVertex3;     m_cubeVertexColorDynamicVector[vertexOffset + 3] = particle.finalColor;

                m_cubeVertexPosDynamicVector[vertexOffset + 4] = particle.finalVertex4;     m_cubeVertexColorDynamicVector[vertexOffset + 4] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 5] = particle.finalVertex5;     m_cubeVertexColorDynamicVector[vertexOffset + 5] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 6] = particle.finalVertex6;     m_cubeVertexColorDynamicVector[vertexOffset + 6] = particle.finalColor;
                m_cubeVertexPosDynamicVector[vertexOffset + 7] = particle.finalVertex7;     m_cubeVertexColorDynamicVector[vertexOffset + 7] = particle.finalColor;

                vertexOffset += m_cubeVerticesCount;
            }

            m_activeCount += m_activeCubes;

            if(m_activeCubes > 0)
            {
                m_cubeVertexPosDynamicBuffer->setDynamicBufferData(m_cubeVertexPosDynamicVector, m_activeCubes * m_cubeVerticesCount);
                m_cubeVertexColorDynamicBuffer->setDynamicBufferData(m_cubeVertexColorDynamicVector, m_activeCubes * m_cubeVerticesCount);
                m_cubeIndexBuffer->setCount(m_activeCubes * m_cubeIndicesCount);

                m_internalShader->bind();
                m_cubeVertexArray->bind();
                m_cubeVertexArray->draw();
            }
        }

        if(m_activeCubes == 0)
            m_anyCubeParticleIsActive = false;
    }

    void ParticleSystem::disableAll()
    {
        BR_INFO("%s", "ParticleSystem::disableAll()");

        for(QuadParticle& particle : m_quadParticles)
            particle.isActive = false;

        for(CubeParticle& particle : m_cubeParticles)
            particle.isActive = false;

        m_currentQuadParticlesIndex = static_cast<int>(m_quadParticles.size()) - 1;
        m_currentCubeParticlesIndex = static_cast<int>(m_cubeParticles.size()) - 1;
    }

    void ParticleSystem::EmitQuadsFromCenter(int particlesCount,
                                             float lifeTimeSec,
                                             float sizeBegin,
                                             float sizeEnd,
                                             const glm::vec4& colorBegin,
                                             const glm::vec4& colorEnd,
                                             const glm::vec3& pos,
                                             const glm::vec3& gravity,
                                             float speed)
    {
        BR_ASSERT((!m_quadParticles.empty() && !m_cubeParticles.empty()), "%s", "Create Particle system before use.");

        for(int i = 0; i < particlesCount; ++i)
        {
            m_anyQuadParticleIsActive = true;

            QuadParticle& particle = m_quadParticles[m_currentQuadParticlesIndex];
            --m_currentQuadParticlesIndex;
            if(m_currentQuadParticlesIndex < 0)
            {
                m_currentQuadParticlesIndex = static_cast<int>(m_quadParticles.size()) - 1;
            }

            particle.isActive = true;

            particle.lifeTimeFull = lifeTimeSec;
            particle.lifeTimePassed = 0.0f;

            particle.sizeBegin = sizeBegin + sizeBegin * ((RandomGenerator::getFloat() - 0.5f) * 0.5f);
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

    void ParticleSystem::EmitCubesFromCenter(int particlesCount,
                                             float lifeTimeSec,
                                             float sizeBegin,
                                             float sizeEnd,
                                             const glm::vec4& colorBegin,
                                             const glm::vec4& colorEnd,
                                             const glm::vec3& pos,
                                             const glm::vec3& gravity,
                                             float speed)
    {
        BR_ASSERT((!m_quadParticles.empty() && !m_cubeParticles.empty()), "%s", "Create Particle system before use.");

        for(int i = 0; i < particlesCount; ++i)
        {
            m_anyCubeParticleIsActive = true;

            CubeParticle& particle = m_cubeParticles[m_currentCubeParticlesIndex];
            --m_currentCubeParticlesIndex;
            if(m_currentCubeParticlesIndex < 0)
            {
                m_currentCubeParticlesIndex = static_cast<int>(m_cubeParticles.size()) - 1;
            }

            particle.isActive = true;

            particle.lifeTimeFull = lifeTimeSec;
            particle.lifeTimePassed = 0.0f;

            particle.sizeBegin = sizeBegin + sizeBegin * ((RandomGenerator::getFloat() - 0.5f) * 0.5f);
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
