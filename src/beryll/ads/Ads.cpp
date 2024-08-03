#include "Ads.h"

#if defined(ANDROID)
    #include "beryll/platform/androidGLES/AndroidGLESAds.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<Ads> Ads::m_ads = nullptr;

    void Ads::create()
    {
        if(m_ads) { return; }

#if defined(ANDROID)
        m_ads = std::unique_ptr<Ads>(new AndroidGLESAds());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create Ads. Unknown platform.");
#endif
    }
}
