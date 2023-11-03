#include "BillingSystem.h"

#if defined(ANDROID)
    #include "beryll/platform/androidGLES/AndroidGLESBillingSystem.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<BillingSystem> BillingSystem::m_billingSystem = nullptr;

    void BillingSystem::create()
    {
        if(m_billingSystem) { return; }

#if defined(ANDROID)
        m_billingSystem = std::unique_ptr<BillingSystem>(new AndroidGLESBillingSystem());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create BillingSystem. Unknown platform.");
#endif
    }
}
