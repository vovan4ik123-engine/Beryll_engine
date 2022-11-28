#include "ParticleSystem.h"

#if defined(ANDROID)
    #include "Beryll/Platform/AndroidGLES/AndroidGLESParticleSystem.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<ParticleSystem> ParticleSystem::m_system = nullptr;

    const std::vector<glm::vec4> ParticleSystem::quadVertices{glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
                                                              glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
                                                              glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
                                                              glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)};

    const std::vector<glm::vec4> ParticleSystem::cubeVertices{glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), // front side +Z
                                                              glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
                                                              glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                                                              glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),

                                                              glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // back side -Z
                                                              glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
                                                              glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
                                                              glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f)};

    void ParticleSystem::create()
    {
        if(m_system) { return; }

#if defined(ANDROID)
        m_system = std::unique_ptr<ParticleSystem>(new AndroidGLESParticleSystem());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create ParticleSystem. Unknown platform.");
#endif
    }
}
