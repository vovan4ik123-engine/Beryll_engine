#include "GoogleAnalytics.h"

#if defined(ANDROID)
    #include "beryll/platform/androidGLES/AndroidGLESGoogleAnalytics.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<GoogleAnalytics> GoogleAnalytics::m_googleAnalytics = nullptr;

    void GoogleAnalytics::create()
    {
        if(m_googleAnalytics) { return; }

#if defined(ANDROID)
        m_googleAnalytics = std::unique_ptr<GoogleAnalytics>(new AndroidGLESGoogleAnalytics());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create GoogleAnalytics. Unknown platform.");
#endif
    }
}
