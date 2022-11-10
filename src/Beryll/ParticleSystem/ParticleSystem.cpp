#include "ParticleSystem.h"

#if defined(ANDROID)
    #include "Beryll/Platform/AndroidGLES/AndroidGLESParticleSystem.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<ParticleSystem> ParticleSystem::m_system = nullptr;

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
